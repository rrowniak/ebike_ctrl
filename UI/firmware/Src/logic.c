#include "logic.h"
#include "main.h"
#include "version.h"
#include "state.h"
#include "system.h"
#include "ui.h"

#include <lrr_hd44780.h>
#include <lrr_usart.h>
#include <lrr_timer.h>
#include <lrr_utils.h>
#include <lrr_eeprom_24LC256.h>
#include <bike_can_protocol.h> 

#include <string.h>

extern CAN_HandleTypeDef hcan;

static CAN_FilterTypeDef can_filter;

static struct vehicle_conf vc;
static struct vehicle_runtime vr;
static struct vehicle_gauges vg;

static struct Timer tim30s = { .Period_ms = 30000, .Prev_ms = 0};
static struct Timer tim10s = { .Period_ms = 10000, .Prev_ms = 0};
static struct Timer tim1s = { .Period_ms = 1000, .Prev_ms = 0};
static struct Timer tim05s = { .Period_ms = 500, .Prev_ms = 0};
static struct Timer tim20ms = { .Period_ms = 20, .Prev_ms = 0};

static uint32_t total_pulses = 0;
static uint32_t prev_pulses = 0;
static uint32_t prev_pulses_timestamp = 0;

static uint32_t prev_electric_timestamp = 0;
static float consumed_Ws = 0.0;
static float recovered_Ws = 0.0;


static uint8_t inactivity_watchdog = 0;
static uint8_t any_movement_detected = 0;

static uint16_t btn_1_watchdog = 0;
static uint16_t btn_2_watchdog = 0;
static uint16_t btn_3_watchdog = 0;

static uint8_t beep_cnt = 0;

static uint16_t motherboard_watchdog = 0;
static uint8_t first_motherboard_el_update = 1;

static inline uint32_t _convert_to_mm(const struct vehicle_conf* vc, 
    uint32_t pulses)
{
    return pulses * vc->dist_p_rev_mm / vc->pulse_p_rev;
}

static inline uint32_t _convert_to_m(const struct vehicle_conf* vc,
    uint32_t pulses)
{
    return _convert_to_mm(vc, pulses) / 1000;
}

static void _load_config(
    struct vehicle_conf* vc,
    struct vehicle_runtime* vr
    )
{
    struct eeprom_constants ec;
    int err;

    LOG("Loading config from eeprom...");

    err = load_eeprom_constants(&ec);
    if (err) {
        LOG("Loading eeprom constants failed!");
        return;
    }

    if (!check_eeprom_constants(&ec)) {
        // virgin EEPROM
        LOG("Virgin mode");
        init_eeprom_constants(&ec);

        init_vehicle_conf(vc);
        init_vehicle_runtime(vr);

        err = save_eeprom_constants(&ec);

        if (err) {
            LOG("Saving eeprom constants failed!");
            return;
        }
        err = save_vehicle_conf(vc);

        if (err) {
            LOG("Saving vehicle constants failed!");
            return;
        }
        err = save_vehicle_runtime(vr);

        if (err) {
            LOG("Saving vehicle runtime failed!");
            return;
        }
    } else {
        err = load_vehicle_conf(vc);
        if (err) {
            LOG("Loading eeprom vehicle conf failed!");
            return;
        }

        err = load_vehicle_runtime(vr);
        if (err) {
            LOG("Loading eeprom vehicle runtime failed!");
            return;
        }

        // initial vehicle gauge init
        vg.total_m = _convert_to_m(vc, vr->total.dist_pulses);
        vg.trip1_m = _convert_to_m(vc,
            vr->total.dist_pulses - vr->trip1.dist_pulses);
        vg.trip2_m = _convert_to_m(vc,
            vr->total.dist_pulses - vr->trip2.dist_pulses); 

    }

    LOG("SUCCESS.");
}

void logic_init(void)
{
    ui_init();

    ui_welcome_screen_blk_1();

    LOG("Logic init...");

    _load_config(&vc, &vr);

    ui_welcome_screen_blk_2();

    LOG("Logic done.");

    if (is_btn_pressed_pin_check(BUTTON_1) && is_btn_pressed_pin_check(BUTTON_2)) {
        ui_initial_setup(&vc);
        save_vehicle_conf(&vc);
    }

    can_filter.FilterMode = CAN_FILTERMODE_IDMASK;
    can_filter.FilterScale = CAN_FILTERSCALE_32BIT;
    can_filter.FilterIdHigh = 0x0000;
    can_filter.FilterIdLow = 0x0000;
    can_filter.FilterMaskIdHigh = 0x0000;
    can_filter.FilterMaskIdLow = 0x0000;
    can_filter.FilterFIFOAssignment = CAN_FILTER_FIFO0;
    can_filter.FilterActivation = CAN_FILTER_ENABLE;
    can_filter.FilterBank = 0;

    HAL_StatusTypeDef ret;

    ret = HAL_CAN_ConfigFilter(&hcan, &can_filter);
    if (ret != HAL_OK) {
        LOG2("Fail HAL_CAN_ConfigFilter ", ret);
    }

    ret = HAL_CAN_Start(&hcan);
    if (ret != HAL_OK) {
        LOG2("Fail HAL_CAN_Start ", ret);
    }

    vg.ambient_temp = readTemp();
}

static inline uint32_t timestamp_delta(uint32_t prev, uint32_t curr)
{
    if (prev <= curr) {
        return curr - prev;
    } else {
        // wrap around arithmetics
        return MAX_TIMESTAMP - prev + curr;
    }
}

void logic_update(void)
{
    uint32_t now_ms = HAL_GetTick();

    // check if there any messages waiting on CAN bus
    uint8_t data[8];
    CAN_RxHeaderTypeDef can_header;
    while (HAL_CAN_GetRxFifoFillLevel(&hcan, CAN_RX_FIFO0) > 0) {
        HAL_StatusTypeDef ret;
        ret = HAL_CAN_GetRxMessage(&hcan, CAN_RX_FIFO0, &can_header, data);
        if (ret != HAL_OK) {
            LOG("CAN ERROR");
            continue;
        }
        motherboard_watchdog = 0;
        // process message
        switch (data[0])
        {
        case BCP_MSG_ELECTRIC:
        {
            const struct bcp_msg_electric* el 
                = (const struct bcp_msg_electric*)&data[1];

            vg.batt_v = el->voltage;
            vg.batt_v /= 10;
            // calculate percentage
            int v_max_mv = vc.batt_s * vc.cell_mv_max;
            int v_low_mv = vc.batt_s * vc.cell_mv_min;
            float batt_perc = (vg.batt_v * 1000 - v_low_mv) / (v_max_mv - v_low_mv);
            vg.batt_perc = 100.0 * (batt_perc < 0 ? 0 : batt_perc);

            vr.last_batt_mv = el->voltage * 100;
            vg.amper = convert_from_14bit(el->current);

            if (vc.reverse_curr) {
                vg.amper /= -10;
            } else {
                vg.amper /= 10;
            }

            if (first_motherboard_el_update) {
                first_motherboard_el_update = 0;
                // ampere sanity check
                if (vg.amper > 30.0 || vg.amper < -30.0) {
                    // perhaps the sensor is not installed or corrupted
                    // TODO: the motherboard should report dedicated fault
                    ui_disable_amp_gauges();
                }
            }

            uint32_t delta_t_ms = timestamp_delta(prev_electric_timestamp, el->timestamp);
            prev_electric_timestamp = el->timestamp;
            // update consumedWH and recovered_Ws
            float Ws = vg.amper * vg.batt_v * delta_t_ms / 1000.0;

            if (vg.amper > 0) {
                consumed_Ws += Ws;
            } else {
                recovered_Ws += Ws;
            }
            break;
        }
        case BCP_MSG_MOTION:
        {
            const struct bcp_msg_motion* m 
                = (const struct bcp_msg_motion*)&data[1];
            total_pulses = m->tot_pulses;

            // convert to distance in mili-meters
            uint32_t delta_mm = _convert_to_mm(&vc,
                m->tot_pulses - prev_pulses);
            
            // calculate delta t
            uint32_t delta_t_ms = timestamp_delta(prev_pulses_timestamp, m->timestamp);

            delta_t_ms = (delta_t_ms == 0) ? 1 : delta_t_ms;

            vg.speed_kmh = 36 * delta_mm / (10 * delta_t_ms);
            vg.total_m = _convert_to_m(&vc, 
                vr.total.dist_pulses + total_pulses);
            vg.trip1_m = _convert_to_m(&vc,
                vr.total.dist_pulses + total_pulses - vr.trip1.dist_pulses);
            vg.trip2_m = _convert_to_m(&vc,
                vr.total.dist_pulses + total_pulses - vr.trip2.dist_pulses); 

            prev_pulses = m->tot_pulses;
            prev_pulses_timestamp = m->timestamp;
            break;
        }
        case BCP_MSG_SENS_BLK1:
        {
            const struct bcp_msg_sens_blk1* blk
                = (const struct bcp_msg_sens_blk1*)&data[1];
            vg.moto_temp = convert_from_9bit(blk->moto_t);
            vg.driver_temp = convert_from_9bit(blk->drv_t);
            vg.batt_temp = convert_from_9bit(blk->batt_t);
            break;
        }
        default:
            break;
        }
    }

    if (__timer_update(&tim20ms, now_ms)) {
        
        uint8_t lock_display_mode = 0;
        
        if (is_btn_pressed(BUTTON_3)) {
            ++btn_3_watchdog;
        } else {
            btn_3_watchdog = 0;
        }

        if (is_btn_pressed(BUTTON_2)) {
            ++btn_2_watchdog;
        } else {
            btn_2_watchdog = 0;
        }

        if (is_btn_pressed(BUTTON_1)) {
            ++btn_1_watchdog;
        } else {
            btn_1_watchdog = 0;
        }

        if (btn_3_watchdog == 50) {
            lcd_backlight_toogle();
            lock_display_mode = 1;
        }

        if (btn_1_watchdog == 50) {
            // reset trip 1
            vr.trip1.dist_pulses = vr.total.dist_pulses + total_pulses;
        }

        if (btn_2_watchdog == 50) {
            // reset trip 2
            vr.trip2.dist_pulses = vr.total.dist_pulses + total_pulses;
        }

        if (get_n_reset_btn_released(BUTTON_3) && !lock_display_mode) {            
            // advance display mode
            ++vr.current_display_mode;
            if (vr.current_display_mode >= DM_LIMIT) {
                vr.current_display_mode = 0;
            }
            ui_set_display_mode((enum display_mode)vr.current_display_mode);
        }

        if (beep_cnt > 0) {
            --beep_cnt;

            if (beep_cnt == 0) {
                beep_off();
            } else {
                beep_on();
            }
        }
    }

    if (__timer_update(&tim05s, now_ms)) {
        if (motherboard_watchdog > 3) {
            vg.motherboard_offline = 1;
        } else {
            vg.motherboard_offline = 0;
        }
        
        // update UI
        ui_update(&vg);

        if (vg.speed_kmh != 0) {
            inactivity_watchdog = 0;
            any_movement_detected = 1;
        }
    }

    if (__timer_update(&tim1s, now_ms)) {
        ++motherboard_watchdog;
        if (vg.speed_kmh == 0) {
            ++inactivity_watchdog;
        }

        if (inactivity_watchdog == 1 && any_movement_detected) {
            // save runtime to EEPROM
            // LOG("Saving state to eeprom");
            uint32_t old_dist_pulses = vr.total.dist_pulses;
            vr.total.dist_pulses += total_pulses;
            save_vehicle_runtime(&vr);
            // the trip might get continued so we need to use old value
            vr.total.dist_pulses = old_dist_pulses;
            // LOG("conf saved to EEPROM");
        }

        if (any_movement_detected && inactivity_watchdog == 60) {
            beep_cnt = inactivity_watchdog;
        }
    }

    if (__timer_update(&tim10s, now_ms)) {
        // update consumed/recovered Wh
        float traveled_km = vg.total_m / 1000.0;

        if (traveled_km > 0.01) {
            vg.consumed_Wh += (consumed_Ws / 3600.0);
            vg.brake_Wh += (recovered_Ws / 3600.0);
            vg.Wh_km = (vg.consumed_Wh - vg.brake_Wh) / traveled_km;

            consumed_Ws = 0;
            recovered_Ws = 0;
        }
    }

    if (__timer_update(&tim30s, now_ms)) {
        vg.ambient_temp = readTemp();        
    }
}
