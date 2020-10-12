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

#include "system.h"
#include "version.h"
#include "main.h"
#include <lrr_hd44780.h>
#include <lrr_usart.h>
#include <lrr_lm35.h>
#include <lrr_kty8x.h>
#include "stm32f1xx_hal.h"

#include <lrr_eeprom_24LC256.h>

extern ADC_HandleTypeDef hadc1;
extern I2C_HandleTypeDef hi2c1;
extern UART_HandleTypeDef huart1;

static volatile uint16_t button_states = 0;

#define BUTTON_ON           0x01
#define BUTTON_PRESSED_EV   0x02
#define BUTTON_RELEASED_EV  0x04

void reset_button_states(void)
{
    button_states = 0;
}

uint8_t is_btn_pressed_pin_check(uint16_t btn_no)
{
    switch (btn_no) {
    case BUTTON_1:
        return HAL_GPIO_ReadPin(S1_GPIO_Port, S1_Pin) == GPIO_PIN_RESET;
    case BUTTON_2:
        return HAL_GPIO_ReadPin(S2_GPIO_Port, S2_Pin) == GPIO_PIN_RESET;
    case BUTTON_3:
        return HAL_GPIO_ReadPin(S3_GPIO_Port, S3_Pin) == GPIO_PIN_RESET;
    }

    return 0;
}

uint8_t is_btn_pressed(uint16_t btn_no)
{
    return (button_states & (BUTTON_ON << btn_no)) > 0;
}

uint8_t get_n_reset_btn_pressed(uint16_t btn_no)
{
    uint8_t r = (button_states & (BUTTON_PRESSED_EV << btn_no)) > 0;
    button_states &= ~(BUTTON_PRESSED_EV << btn_no);
    return r;
}

uint8_t get_n_reset_btn_released(uint16_t btn_no)
{
    uint8_t r = (button_states & (BUTTON_RELEASED_EV << btn_no)) > 0;
    button_states &= ~(BUTTON_RELEASED_EV << btn_no);
    return r;
}

static uint8_t peripheral_state = 0;

#define PER_STATE_BEEP      0x01
#define PER_STATE_LIGHT     0x02

void system_init(void)
{
    usart_config(&huart1);
    eeprom_24lc256_init(&hi2c1);
}

void panic(const char* msg)
{
    lcd_println("PANIC FW:v" VERSION, 0);
    lcd_println(msg, 1);
    while (1);
}

int8_t readTemp(void)
{
    HAL_ADC_Start(&hadc1);

    uint16_t sum = 0;

    for (int i = 0; i < 3; ++i) {
        HAL_StatusTypeDef ret = HAL_ADC_PollForConversion(&hadc1, 50);
        if (ret != HAL_OK) {
            return -30 - ret; 
        }
        sum += HAL_ADC_GetValue(&hadc1);
        HAL_Delay(3);
    }

    HAL_ADC_Stop(&hadc1);

    float adc_v = V_REF * sum / (float)(3 * 4095);
    
    // dummy error correction
    adc_v += 0.2;
    // LOG2("readTemp done: ", (int32_t)(adc_v*1000));

    float rt = adc_v * 2200.0 / (V_REF_5V - adc_v);
    // LOG2("Calculated resistance: ", (uint32_t)(rt*1000));
    return kty81_120_get_temp(rt);
}

void lcd_backlight_on(void)
{
    if (!(peripheral_state & PER_STATE_LIGHT)) {
        HAL_GPIO_WritePin(BACKLIGHT_GPIO_Port, BACKLIGHT_Pin, GPIO_PIN_SET);
        peripheral_state |= PER_STATE_LIGHT;
    }
}

void lcd_backlight_off(void)
{
    if (peripheral_state & PER_STATE_LIGHT) {
        HAL_GPIO_WritePin(BACKLIGHT_GPIO_Port, BACKLIGHT_Pin, GPIO_PIN_RESET);
        peripheral_state &= ~PER_STATE_LIGHT;
    }
}

void lcd_backlight_toogle(void)
{
    HAL_GPIO_TogglePin(BACKLIGHT_GPIO_Port, BACKLIGHT_Pin);
}

void sys_led_on(void)
{
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);
}

void sys_led_off(void)
{
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);
}

void beep_on(void)
{
    if (!(peripheral_state & PER_STATE_BEEP)) {
        HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_SET);
        peripheral_state |= PER_STATE_BEEP;
    }
}

void beep_off(void)
{
    if (peripheral_state & PER_STATE_BEEP) {
        HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_RESET);
        peripheral_state &= ~PER_STATE_BEEP;
    }
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == S3_Pin) {
        LOG("HAL_GPIO_EXTI_Callback");
        if (HAL_GPIO_ReadPin(S3_GPIO_Port, S3_Pin) == GPIO_PIN_SET) {
            // button released
            button_states &= ~(BUTTON_ON << BUTTON_3);
            button_states |= (BUTTON_RELEASED_EV  << BUTTON_3);
        } else {
            // button pressed
            button_states |= ((BUTTON_PRESSED_EV | BUTTON_ON)  << BUTTON_3);
        }
    } else if (GPIO_Pin == S2_Pin) {
        if (HAL_GPIO_ReadPin(S2_GPIO_Port, S2_Pin) == GPIO_PIN_SET) {
            button_states &= ~(BUTTON_ON << BUTTON_2);
            button_states |= (BUTTON_RELEASED_EV  << BUTTON_2);
        } else {
            button_states |= ((BUTTON_PRESSED_EV | BUTTON_ON)  << BUTTON_2);
        }
    } else if (GPIO_Pin == S1_Pin) {
        if (HAL_GPIO_ReadPin(S1_GPIO_Port, S1_Pin) == GPIO_PIN_SET) {
            button_states &= ~(BUTTON_ON << BUTTON_1);
            button_states |= (BUTTON_RELEASED_EV  << BUTTON_1);
        } else {
            button_states |= ((BUTTON_PRESSED_EV | BUTTON_ON)  << BUTTON_1);
        }
    }
}