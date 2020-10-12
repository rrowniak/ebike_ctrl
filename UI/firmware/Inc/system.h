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

#ifndef __SYSTEM_H__
#define __SYSTEM_H__

#include <stdint.h>

#define V_REF 3.3
#define V_REF_MV 3300
#define V_REF_5V 5.0

#define BUTTON_1    0
#define BUTTON_2    4
#define BUTTON_3    8

void reset_button_states(void);
uint8_t is_btn_pressed_pin_check(uint16_t btn_no);
uint8_t is_btn_pressed(uint16_t btn_no);
uint8_t get_n_reset_btn_pressed(uint16_t btn_no);
uint8_t get_n_reset_btn_released(uint16_t btn_no);

void system_init(void);

void panic(const char* message);

int8_t readTemp(void);

void lcd_backlight_on(void);
void lcd_backlight_off(void);
void lcd_backlight_toogle(void);

void sys_led_on(void);
void sys_led_off(void);

void beep_on(void);
void beep_off(void);

#endif // __SYSTEM_H__