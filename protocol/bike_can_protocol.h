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

#ifndef __BIKE_CAN_PROTOCOL_H__
#define __BIKE_CAN_PROTOCOL_H__

#include <lrr_utils.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
    Message format
    -----------------------------------------
    |    |    |    |    |    |    |    |    |
    -----------------------------------------

      B0   B1   B2   B3   B4   B5   B6   B7

    B0 - BCP_MSG_{TYPE}
    B1 - B7 = 7 * 8 = 56
*/

#define BCP_MSG_ELECTRIC      0x01
#define BCP_MSG_MOTION        0x02
#define BCP_MSG_SENS_BLK1     0x03

#define SIGN_MASK_11         0x400
#define UNSIGNED_MASK_11     0x3FF

#define SIGN_MASK_14         0x2000
#define UNSIGNED_MASK_14     0x1FFF

#define SIGN_MASK         0x100
#define UNSIGNED_MASK     0x0FF

static inline int32_t convert_from_11bit(uint32_t v)
{
    if (SIGN_MASK_11 & v) {
      return -(v & UNSIGNED_MASK_11);
    } else {
      return v & UNSIGNED_MASK_11;
    }
}

static inline uint32_t convert_to_11bit(int32_t v)
{
    uint32_t ret = 0;
    if (v < 0) {
        ret |= SIGN_MASK_11;
        v = -v;
        ret += v;
    } else {
        ret += v;
    }
    return ret;
}

#define MAX_TIMESTAMP         0x2000
struct bcp_msg_electric
{
    uint32_t timestamp  : 13;
    // 60.7V = 607
    uint32_t voltage    : 10;
    // 10.8A = 108
    uint32_t current    : 14;
    uint32_t faults     : 11;
    uint32_t seq_id     : 8;
} __attribute__((__packed__));

struct bcp_msg_motion
{
    uint32_t timestamp    : 13;
    uint32_t tot_pulses   : 32;
    uint32_t seq_id       : 8;
    uint32_t reserved     : 2;
} __attribute__((__packed__));

static inline int32_t convert_from_9bit(uint32_t v)
{
    if (SIGN_MASK & v) {
      return -(v & UNSIGNED_MASK);
    } else {
      return v & UNSIGNED_MASK;
    }
}

static inline uint32_t convert_to_9bit(int32_t v)
{
    uint32_t ret = 0;
    if (v < 0) {
        ret |= SIGN_MASK;
        v = -v;
        ret += v;
    } else {
        ret += v;
    }
    return ret;
}

static inline int32_t convert_from_14bit(uint32_t v)
{
    if (SIGN_MASK_14 & v) {
      return -(v & UNSIGNED_MASK_14);
    } else {
      return v & UNSIGNED_MASK_14;
    }
}

static inline uint32_t convert_to_14bit(int32_t v)
{
    uint32_t ret = 0;
    if (v < 0) {
        ret |= SIGN_MASK_14;
        v = -v;
        ret += v;
    } else {
        ret += v;
    }
    return ret;
}

struct bcp_msg_sens_blk1
{
    // most significant bit means a sign
    uint32_t moto_t       : 9;
    uint32_t drv_t        : 9;
    uint32_t batt_t       : 9;
    uint32_t reserved     : 29;
} __attribute__((__packed__));

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
// static_assert(true);
// static_assert(sizeof(bcp_msg_electric) == 7, 
//     "size of bcp_msg_electric != 7");
// static_assert(sizeof(bcp_msg_motion) <= 7, 
//   "size of bcp_msg_motion <= 7");
// static_assert(sizeof(bcp_msg_sens_blk1) <= 7, 
//   "size of bcp_msg_sens_blk1 <= 7");
#else
static_assert(sizeof(struct bcp_msg_electric) == 7, 
    "size of bcp_msg_electric != 7");
static_assert(sizeof(struct bcp_msg_motion) <= 7, 
  "size of bcp_msg_motion <= 7");
static_assert(sizeof(struct bcp_msg_sens_blk1) <= 7, 
  "size of bcp_msg_sens_blk1 <= 7");
#endif

#endif // __BIKE_CAN_PROTOCOL_H__