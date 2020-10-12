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

#include "logic.h"
#include "main.h"
#include "version.h"
#include "can.h"

#include <lrr_usart.h>
#include <lrr_timer.h>
#include <lrr_math.h>
#include <lrr_utils.h>
#include <lrr_kty8x.h>

#include <string.h>

struct adc_conv_results
{
    int32_t moto_t;
    int32_t drv_t;
    int32_t batt_t;

    uint32_t voltage;
    int32_t current;
};

#define CAL_STATUS_FINE             1
#define CAL_STATUS_NEEDED           2
#define CAL_STATUS_DOITNOW          3

#define AMP_SENS_TEST_FAILED        0x01
#define MOTO_KTY83_FAILED           0x02
#define MOTO_NTC_FAILED             0x04
#define BATT_T_SENS_FAILED          0x08
#define DRV_T_SENS_FAILED           0x10

struct self_calibration_results
{
    uint8_t state;
    uint8_t test;
    float current_sens_zero;
};

extern UART_HandleTypeDef huart1;
extern ADC_HandleTypeDef hadc1;

static volatile uint32_t pulse_cnt = 0;

static struct adc_conv_results last_convertion = {
    .moto_t = BAD_TEMP,
    .drv_t = BAD_TEMP,
    .batt_t = BAD_TEMP,
    .voltage = 0,
    .current = 0
};

static struct self_calibration_results calibration = {
    .state = CAL_STATUS_NEEDED,
    .test = 0,
    .current_sens_zero = CURRENT_SENS_ZERO
};

static struct Timer tim5s = { .Period_ms = 5000, .Prev_ms = 0};
static struct Timer tim05s = { .Period_ms = 500, .Prev_ms = 0};
static struct Timer tim50ms = { .Period_ms = 50, .Prev_ms = 0};

static volatile uint8_t convCompleted = 0;

static float v_err_map[][2] = 
{
    {30,        32.5},
    {60.0,      62.3},
    {97.0,      100.3}                
};

static int16_t _conv_temp_KTY81(uint16_t adc)
{
    float adc_v = V_REF * (float)adc / ADC_RES;

    if (adc_v < BAD_TEMP_THRESHOLD) {
        return BAD_TEMP;
    }

    float rt = adc_v * KTY81_RES / (KTY81_VREF - adc_v);

    return kty81_120_get_temp(rt);
}

static int16_t _conv_temp_NTC(uint16_t adc)
{
    float adc_v = (((float)adc) / ADC_RES) * V_REF;

    if (adc_v < BAD_TEMP_THRESHOLD) {
        return BAD_TEMP;
    }

    float rt = adc_v * NTC_RES/ (NTC_VREF - adc_v);

    return ntc_get_temp(rt);
}

static int16_t _conv_moto_temp(uint16_t adc)
{
    float adc_v = V_REF * adc / ADC_RES;

    if (adc_v < BAD_TEMP_THRESHOLD) {
        return BAD_TEMP;
    }

    float rt = adc_v * 17000.0 / (5.0 - adc_v);

    return kty83_122_get_temp(rt);
}

static int32_t _conv_current(uint16_t adc)
{
    float v = V_REF * (float)adc / ADC_RES;

    if (calibration.state == CAL_STATUS_DOITNOW) {
        // the assumption is that no significant current is being drawn
        if ((v > CURRENT_SENS_ZERO + CURRENT_SANITY_CAL_A)
            || (v < CURRENT_SENS_ZERO - CURRENT_SANITY_CAL_A)) {
            // something is wrong
            calibration.test |= AMP_SENS_TEST_FAILED;
        } else {
            calibration.current_sens_zero = v;
        }
    }

    v -= calibration.current_sens_zero;
    v *= 1000;
    v /= CURRENT_SENS_mVA;

    // sanity check
    if (v <= CURRENT_SANITY_A && v >= -CURRENT_SANITY_A) {
        v = 0.0;
    }

    return v * 10;
}

static int32_t _conv_voltage(uint16_t adc)
{
    float v = V_REF * adc / ADC_RES;
    v = v * (BATT_V_DIV_R1 + BATT_V_DIV_R2) / BATT_V_DIV_R2;
    // TODO: recalibration needed!
    // LOG2("Voltage measured: ", v * 10);
    v = lrr_get_corrected(v_err_map, 
        sizeof(v_err_map) / sizeof(v_err_map[0]),
        v);
    return v * 10;
}

void int_conv_dma(DMA_HandleTypeDef *hdma)
{
    if (hdma->State == HAL_DMA_STATE_READY) {
        convCompleted = 1;
    }
}

static void _read_all_adc(void)
{
    uint16_t rawValues[6];
    
    convCompleted = 0;
    
    HAL_ADC_Start_DMA(&hadc1, (uint32_t*)rawValues, 6);

    while (!convCompleted);

    HAL_ADC_Stop_DMA(&hadc1);

    // do unit conversions
    last_convertion.current = _conv_current(rawValues[0]);
    last_convertion.batt_t = _conv_temp_KTY81(rawValues[1]);
    last_convertion.drv_t = _conv_temp_KTY81(rawValues[2]);
    last_convertion.voltage = _conv_voltage(rawValues[3]);

    if (calibration.state == CAL_STATUS_FINE) {
        // check which sensor is available
        if (!(calibration.test & MOTO_KTY83_FAILED)) {
            last_convertion.moto_t = _conv_moto_temp(rawValues[4]);
        } else if (!(calibration.test & MOTO_NTC_FAILED)) {
            last_convertion.moto_t = _conv_temp_NTC(rawValues[5]);
        }
    } else if (calibration.state == CAL_STATUS_NEEDED) {
        // we don't know yet which sensor is connected
        last_convertion.moto_t = BAD_TEMP;
    } else if (calibration.state == CAL_STATUS_DOITNOW) {
        int tmp;

        tmp = _conv_moto_temp(rawValues[4]);
        if (tmp == BAD_TEMP) {
            calibration.test |= MOTO_KTY83_FAILED;
        }

        tmp = _conv_temp_NTC(rawValues[5]);
        if (tmp == BAD_TEMP) {
            calibration.test |= MOTO_NTC_FAILED;
        }
    }


    if (calibration.state == CAL_STATUS_DOITNOW) {
        // done
        calibration.state = CAL_STATUS_FINE;
    }
}

void logic_init(void)
{
    usart_config(&huart1);

    LOG("Init");

    can_init();

    HAL_ADC_Start(&hadc1);
}

void logic_update(void)
{
    uint32_t now_ms = HAL_GetTick();    

    if (__timer_update(&tim50ms, now_ms)) {
        // measure electric units
        _read_all_adc();
        can_send_electric(last_convertion.voltage, last_convertion.current);
    }

    if (__timer_update(&tim05s, now_ms)) {
        if (calibration.state == CAL_STATUS_NEEDED) {
            // 0.5 sec should be enough to charge all capacitors so the current
            // should have stabilized arond zero
            calibration.state = CAL_STATUS_DOITNOW;
        }
        // measure distance, send electric units + dist
        can_send_motion(pulse_cnt);
    }

    if (__timer_update(&tim5s, now_ms)) {
        // measure temp & send
        can_send_temp(last_convertion.moto_t, last_convertion.drv_t, last_convertion.batt_t);
    } 
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == HALL_IN_Pin) {
        ++pulse_cnt;
    }
}