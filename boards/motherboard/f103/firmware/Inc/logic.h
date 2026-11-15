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

#ifndef __LOGIC_H__
#define __LOGIC_H__

#include <stm32f1xx.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// configuration
#define V_REF               3.3
#define ADC_RES             4096
#define V_REF_5V            5.0

#define BATT_V_DIV_R1       100000
#define BATT_V_DIV_R2       3300

// for ACS770KCB-150B
// #define CURRENT_SENS_mVA    13.3

// for ACS770LCB-100B
#define CURRENT_SENS_mVA    20.0
#define CURRENT_SENS_ZERO   (V_REF_5V / 2)
#define CURRENT_SANITY_A    0.4
#define CURRENT_SANITY_CAL_A 1.0

#define KTY81_VREF          V_REF_5V
#define KTY81_RES           2200

#define NTC_VREF            V_REF_5V
#define NTC_RES             10000

#define BAD_TEMP            -1000
#define BAD_TEMP_THRESHOLD  0.5

void logic_init(void);
void logic_update(void);

void int_conv_dma(DMA_HandleTypeDef *hdma);

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);

#ifdef __cplusplus
}
#endif

#endif // __LOGIC_H__