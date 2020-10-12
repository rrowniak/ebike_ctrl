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

#include "state.h"
#include "version.h"

#include <lrr_eeprom_24LC256.h>

#include <string.h>

#define EEPROM_MAGIC 0xabd131

#define EEPROM_PAGE             0x0400

#define EEPROM_CONSTANTS_CONF   (EEPROM_PAGE * 0)
#define EEPROM_VEHICLE_CONF     (EEPROM_PAGE * 1)
#define EEPROM_RUNTIME_1        (EEPROM_PAGE * 2)
#define EEPROM_FAULTS           (EEPROM_PAGE * 3)

#define DEF_BATT_P      17
#define DEF_BATT_S      20
#define DEF_CELL_CAP    2850
#define DEF_CELL_V_MAX  4200
#define DEF_CELL_V_MIN  3200
#define DEF_PPR         16
#define DEF_DPR         1830
#define DEF_MOTO_T      90
#define DEF_BATT_T      60
#define DEF_DRV_T       90

void init_eeprom_constants(struct eeprom_constants* ec)
{
    ec->magic = EEPROM_MAGIC;
    strcpy(ec->version, VERSION);
}

int check_eeprom_constants(const struct eeprom_constants* ec)
{
    int ret = 1;
    ret &= (ec->magic == EEPROM_MAGIC);
    // ret &= (strcmp(ec->version, VERSION) == 0);
    return ret;
}

int load_eeprom_constants(struct eeprom_constants* ec)
{
    HAL_StatusTypeDef ret;
    
    ret = eeprom_24lc256_read(EEPROM_CONSTANTS_CONF, 
        (uint8_t*)ec, sizeof(struct eeprom_constants));
    
    if (ret == HAL_OK) {
        return 0;
    }

    return 1;
}

int save_eeprom_constants(const struct eeprom_constants* ec)
{
    HAL_StatusTypeDef ret;
    
    ret = eeprom_24lc256_write(EEPROM_CONSTANTS_CONF, 
        (const uint8_t*)ec, sizeof(struct eeprom_constants));
    
    if (ret == HAL_OK) {
        return 0;
    }

    return 1;
}

void init_vehicle_conf(struct vehicle_conf* vc)
{
    vc->batt_p = DEF_BATT_P;
    vc->batt_s = DEF_BATT_S;
    vc->cell_cap_mah = DEF_CELL_CAP;
    vc->cell_mv_max = DEF_CELL_V_MAX;
    vc->cell_mv_min = DEF_CELL_V_MIN;
    vc->reverse_curr = 0;
    vc->pulse_p_rev = DEF_PPR;
    vc->dist_p_rev_mm = DEF_DPR;
    vc->moto_t_alarm_c = DEF_MOTO_T;
    vc->batt_t_alarm_c = DEF_BATT_T;
    vc->drv_t_alarm_c = DEF_DRV_T;
}

int load_vehicle_conf(struct vehicle_conf* vc)
{
    HAL_StatusTypeDef ret;
    
    ret = eeprom_24lc256_read(EEPROM_VEHICLE_CONF, 
        (uint8_t*)vc, sizeof(struct vehicle_conf));
    
    if (ret == HAL_OK) {
        return 0;
    }

    return 1;
}

int save_vehicle_conf(const struct vehicle_conf* vc)
{
    HAL_StatusTypeDef ret;
    
    ret = eeprom_24lc256_write(EEPROM_VEHICLE_CONF, 
        (const uint8_t*)vc, sizeof(struct vehicle_conf));
    
    if (ret == HAL_OK) {
        return 0;
    }

    return 1;
}

void init_vehicle_runtime(struct vehicle_runtime* vr)
{
    memset(vr, 0, sizeof(struct vehicle_runtime));
}

int load_vehicle_runtime(struct vehicle_runtime* vr)
{
    HAL_StatusTypeDef ret;
    
    ret = eeprom_24lc256_read(EEPROM_RUNTIME_1, 
        (uint8_t*)vr, sizeof(struct vehicle_runtime));
    
    if (ret == HAL_OK) {
        return 0;
    }

    return 1;
}

int save_vehicle_runtime(const struct vehicle_runtime* vr)
{
    HAL_StatusTypeDef ret;
    
    ret = eeprom_24lc256_write(EEPROM_RUNTIME_1, 
        (const uint8_t*)vr, sizeof(struct vehicle_runtime));
    
    if (ret == HAL_OK) {
        return 0;
    }

    return 1;
}