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

#ifndef __UI_H__
#define __UI_H__

#include "state.h"

#ifdef __cplusplus
extern "C" {
#endif

enum display_mode
{
    // "70 km/h 30000.0k"
    // "84.1V 100% +80A "
    DM_DEFAULT,
    DM_TRIP1,
    DM_TRIP2,
    // "25C 80C 30C 28C "
    // "84.1V 100% +80A "
    DM_TEMP,
    // "                "
    // "-1278Ah +100Ah  "
    // "84.1V 100% -80A "
    DM_POWER,
    // "                "
    // "+4208W 108W/km  "
    // "84.1V 100% +80A "
    DM_POWER2,
    DM_LIMIT,
};

struct vehicle_gauges
{
    uint8_t motherboard_offline;
    float batt_v;
    uint8_t batt_perc;
    float amper;

    // in meters
    uint32_t total_m;
    uint32_t trip1_m;
    uint32_t trip2_m;

    uint8_t speed_kmh;
    
    int16_t ambient_temp;
    int16_t moto_temp;
    int16_t driver_temp;
    int16_t batt_temp;

    float consumed_Wh;
    float brake_Wh;
    float Wh_km;
};

void ui_init(void);

void ui_set_display_mode(enum display_mode dm);

void ui_disable_amp_gauges(void);

void ui_update(const struct vehicle_gauges* vg);

void ui_welcome_screen_blk_1(void);

void ui_welcome_screen_blk_2(void);

void ui_initial_setup(struct vehicle_conf* vc);

#ifdef __cplusplus
}
#endif

#endif // __UI_H__