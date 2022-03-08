/*
 * Copyright (c) 2020 Rafal Rowniak rrowniak.com
 * 
 * The author hereby grant you a non-exclusive, non-transferable,
 * free of charge right to copy, modify, merge, publish and distribute,
 * the Software for the sole purpose of performing non-commercial
 * scientific research, non-commercial education, or non-commercial 
 * artistic projects.
 * 
 * Any other use, in particular any use for commercial purposes,
 * is prohibited. This includes, without limitation, incorporation
 * in a commercial product, use in a commercial service, or production
 * of other artefacts for commercial purposes.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR
 * THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 * 
 */

#include "ui.h"
#include "system.h"
#include "version.h"
#include <lrr_hd44780.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#define MAX_LINE 16

static enum display_mode mode;
static uint8_t current = 1;
static char lcd_line[17];

void ui_init(void)
{
    lcd_init();
    lcd_on();
    lcd_clear();
    lcd_disable_cursor();
}

static void clean_line_buffer(void)
{
    for (int i = 0; i < MAX_LINE; ++i) {
        lcd_line[i] = ' ';
    }
    lcd_line[MAX_LINE] = '\0';
}

static void unit_2_line(float v, char unit, int latest_char)
{
    if (v < 0.1) {
        lcd_line[latest_char - 1] = '0';
    } else if (v < 10.0) {
        sprintf(&lcd_line[latest_char - 3], "%.1f", v);
    } else if (v < 100.0) {
        sprintf(&lcd_line[latest_char - 4], "%.1f", v);
    } else if (v < 1000.0) {
        sprintf(&lcd_line[latest_char - 3], "%.0f", v);
    } else if (v < 10000.0) {
        v /= 1000.0;
        sprintf(&lcd_line[latest_char - 4], "%.1fk", v);
    } else {
        v /= 1000.0;
        sprintf(&lcd_line[latest_char - 3], "%.0fk", v);
    }

    lcd_line[latest_char] = unit;
}

static void unit_2_line_int(unsigned int v, char unit, int latest_char)
{
    if (v < 10) {
        sprintf(&lcd_line[latest_char - 1], "%d", v);
    } else if (v < 100) {
        sprintf(&lcd_line[latest_char - 2], "%d", v);
    } else if (v < 1000) {
        sprintf(&lcd_line[latest_char - 3], "%d", v);
    }

    lcd_line[latest_char] = unit;
}

void ui_set_display_mode(enum display_mode dm)
{
    mode = dm;
}

void ui_disable_amp_gauges(void)
{
    current = 0;
}

static void _displ_trip(uint8_t speed, uint32_t dist_m, uint8_t trip_num)
{
    int32_t d_km = dist_m / 1000;
    int32_t d_01 = (dist_m - d_km * 1000) / 100;
    
    if (d_km < 10000) {
        if (trip_num == 0) {
            lcd_printfln("%d km/h %d.%dkm", speed, d_km, d_01);
        } else {
            lcd_printfln("%d km/h %d.%dkm-%d", speed, d_km, d_01, trip_num);
        }
    } else {
        if (trip_num == 0) {
            lcd_printfln("%d km/h %dkm", speed, d_km);
        } else {
            lcd_printfln("%d km/h %dkm-%d", speed, d_km, trip_num);
        }
    }
}

void ui_update(const struct vehicle_gauges* vg)
{
    lcd_set_cursor(0, 1);

    if (!vg->motherboard_offline) {
        if (current) {
            clean_line_buffer();
            unit_2_line(vg->batt_v, 'V', 4);
            unit_2_line_int(vg->batt_perc, '%', 9);
            unit_2_line((vg->amper < 0)? -vg->amper : vg->amper, 'A', 15);
            lcd_printfln("%s", lcd_line);
        } else {
            lcd_printfln("%.1fV %d%% %dC", 
                    vg->batt_v, vg->batt_perc, vg->ambient_temp);
        }
    } else {
        lcd_println("OFFLINE!", 1);
    }

    lcd_set_cursor(0, 0);

    switch (mode)
    {
    case DM_TRIP1:
        _displ_trip(vg->speed_kmh, vg->trip1_m, 1);
        break;
    case DM_TRIP2:
        _displ_trip(vg->speed_kmh, vg->trip2_m, 2);
        break;
    case DM_TEMP:
        lcd_printfln("%dC %dC %dC %dC", vg->ambient_temp
            , (vg->moto_temp < 0) ? 0 : vg->moto_temp
            , (vg->driver_temp < 0) ? 0 : vg->driver_temp
            , (vg->batt_temp < 0) ? 0 : vg->batt_temp);
        break;
    case DM_POWER:
        lcd_printfln("-%.1fWh +%.1fWh", vg->consumed_Wh, vg->brake_Wh);
        break;
    case DM_POWER2:
        clean_line_buffer();
        float a = (vg->amper < 0) ? -vg->amper : vg->amper;
        unit_2_line(a * vg->batt_v, 'W', 5);
        unit_2_line(vg->Wh_km, 'W', 11);
        lcd_line[12] = 'h'; lcd_line[13] = '/'; lcd_line[14] = 'k'; lcd_line[15] = 'm';
        if (vg->amper < 0) lcd_line[0] = '+';
        lcd_printfln("%s", lcd_line);
        // lcd_printfln("%dW %.1fW/km", (int32_t)(vg->amper * vg->batt_v), vg->Wh_km);
        break;
    case DM_DEFAULT:
    default:
        _displ_trip(vg->speed_kmh, vg->total_m, 0);
        break;
    }
}

void ui_welcome_screen_blk_1(void)
{
    lcd_backlight_on();
    lcd_println("Rafal Rowniak", 0);
    lcd_println("  rrowniak.com", 1);
    HAL_Delay(700);
    
    beep_on();
    HAL_Delay(100);
    beep_off();
}

void ui_welcome_screen_blk_2(void)
{

    lcd_println("     BOROWY", 0);
    lcd_println("ver: " VERSION, 1);
    HAL_Delay(700);
    lcd_backlight_off();
}

static uint32_t _ask_user_for_blk_jump_by(uint32_t def, 
    uint32_t min, uint32_t max, uint32_t jump_by, const char* msg)
{
    uint32_t v = def;
    uint8_t refresh = 1;

    lcd_println(msg, 0);
    lcd_set_cursor(0, 1);
    lcd_printfln("  %d", v);

    HAL_Delay(200);

    reset_button_states();

    while (!is_btn_pressed(BUTTON_3)) {

        if (get_n_reset_btn_released(BUTTON_1) 
            && (v >= min + jump_by)) {
            v -= jump_by;
            refresh = 1;
        } else if (get_n_reset_btn_released(BUTTON_2) 
                   && (v + jump_by <= max)) {
            v += jump_by;
            refresh = 1;
        }

        if (refresh) {
            lcd_set_cursor(0, 1);
            lcd_printfln("  %d", v);
            refresh = 0;
        }

        HAL_Delay(20);
    }

    HAL_Delay(100);

    return v;
}

static uint32_t _ask_user_for_blk(uint32_t def, 
    uint32_t min, uint32_t max, const char* msg)
{
    return _ask_user_for_blk_jump_by(def, min, max, 1, msg);
}

void ui_initial_setup(struct vehicle_conf* vc)
{
    lcd_println("INITIAL SETUP", 0);
    lcd_println("PRESS MAIN KEY", 1);
    while (!get_n_reset_btn_released(BUTTON_3));
    HAL_Delay(200);

    vc->batt_s = _ask_user_for_blk(vc->batt_s,
        1, 250, "Battery S:");

    vc->batt_p = _ask_user_for_blk(vc->batt_p,
        1, 250, "Battery P:");
    
    vc->cell_cap_mah = _ask_user_for_blk_jump_by(vc->cell_cap_mah,
        1000, 4000, 50, "Cell cap [mAh]:");

    vc->cell_mv_max = _ask_user_for_blk_jump_by(vc->cell_mv_max,
        3000, 5000, 100, "Cell max [V]:");

    vc->cell_mv_min = _ask_user_for_blk_jump_by(vc->cell_mv_min,
        2500, 4000, 100, "Cell min [V]:");

    vc->reverse_curr = _ask_user_for_blk(vc->reverse_curr,
        0, 1, "Reverse curr:");

    vc->pulse_p_rev = _ask_user_for_blk(vc->pulse_p_rev,
        1, 500, "Pulse per revol:");

    vc->dist_p_rev_mm = _ask_user_for_blk_jump_by(vc->dist_p_rev_mm,
        1000, 3000, 1, "Dist p rev[mm]:");

    vc->moto_t_alarm_c = _ask_user_for_blk(vc->moto_t_alarm_c,
        25, 150, "Moto alarm [C]:");

    vc->batt_t_alarm_c = _ask_user_for_blk(vc->batt_t_alarm_c,
        25, 80, "Batt alarm [C]:");

    vc->drv_t_alarm_c = _ask_user_for_blk(vc->drv_t_alarm_c,
        25, 150, "Driv alarm [C]:");
}