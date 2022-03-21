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
#include "state.h"

#ifdef __cplusplus
extern "C" {
#endif

static inline uint32_t _convert_to_mm(const struct vehicle_conf* vc, 
    uint32_t pulses)
{
    return pulses * vc->dist_p_rev_mm / vc->pulse_p_rev;
}

static inline uint32_t _convert_to_m(const struct vehicle_conf* vc,
    uint32_t pulses)
{
    uint32_t p = pulses / vc->pulse_p_rev;
    // rounding & overflow problems
    if (pulses > 50000000) {
        return (p / 1000) * vc->dist_p_rev_mm;
    } else {
        return p * vc->dist_p_rev_mm / 1000;
    }
}

void logic_init(void);
void logic_update(void);

#ifdef __cplusplus
}
#endif

#endif // __LOGIC_H__