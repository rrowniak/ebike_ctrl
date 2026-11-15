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

#ifndef __STATE_H__
#define __STATE_H__

#include <stdint.h>

struct eeprom_constants
{
    uint32_t magic;
    char version[16];
};

struct vehicle_conf
{
    // battery topology e.g. 16p8s
    uint8_t batt_p;
    uint8_t batt_s;
    // single cell capacity
    uint16_t cell_cap_mah;
    // single cell voltage
    uint16_t cell_mv_max;
    uint16_t cell_mv_min;
    // current interpretation reversed (<0 means regenerative breaking)
    uint8_t reverse_curr;
    // pulse per revolution
    uint16_t pulse_p_rev;
    // distance per revolution
    uint16_t dist_p_rev_mm;
    // motor temp alarm
    uint8_t moto_t_alarm_c;
    // battery temp alarm
    uint8_t batt_t_alarm_c;
    // driver temp alarm
    uint8_t drv_t_alarm_c;
};

struct trip_runtime
{
    // depending on the context it means total pulses
    // or trip starting point
    uint32_t dist_pulses;
    uint32_t travel_time_s;
    uint8_t max_speed_kmh;
    uint32_t consumed_mah;
};

struct vehicle_runtime
{
    uint16_t last_batt_mv;
    uint16_t pow_consumed_mah;
    uint16_t full_batt_charge_cycles;
    uint8_t current_display_mode;
    struct trip_runtime total;
    struct trip_runtime trip1;
    struct trip_runtime trip2;
};

struct faults
{
    uint16_t faults[16];
};

void init_eeprom_constants(struct eeprom_constants* ec);
int check_eeprom_constants(const struct eeprom_constants* ec);
int load_eeprom_constants(struct eeprom_constants* ec);
int save_eeprom_constants(const struct eeprom_constants* ec);

void init_vehicle_conf(struct vehicle_conf* vc);
int load_vehicle_conf(struct vehicle_conf* vc);
int save_vehicle_conf(const struct vehicle_conf* vc);

void init_vehicle_runtime(struct vehicle_runtime* vr);
int load_vehicle_runtime(struct vehicle_runtime* vr);
int save_vehicle_runtime(const struct vehicle_runtime* vr);


#endif // __STATE_H__