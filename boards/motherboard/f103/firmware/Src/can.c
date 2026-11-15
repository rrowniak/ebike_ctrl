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

#include "can.h"

#include <stm32f1xx.h>
#include <bike_can_protocol.h> 
#include <lrr_usart.h>

extern CAN_HandleTypeDef hcan;

static CAN_TxHeaderTypeDef can_header;
static uint8_t electric_seq_id = 0;
static uint8_t motion_seq_id = 0;

static void _send_can(uint8_t data[])
{
    uint32_t mailbox = 0;

    if (HAL_CAN_GetTxMailboxesFreeLevel(&hcan) > 0) {
        // there is a free mailbox
        HAL_StatusTypeDef ret;
        ret = HAL_CAN_AddTxMessage(&hcan, &can_header, data, &mailbox);
        if (ret != HAL_OK) {
            LOG2("Error HAL_CAN_AddTxMessage: ", ret);
        }
    } else {
        LOG("No free mailboxes");
    }
}

void can_init(void)
{
    HAL_StatusTypeDef ret;
    
    ret = HAL_CAN_Start(&hcan);
    
    if (ret != HAL_OK) {
        LOG2("Error HAL_CAN_Start: ", ret);
    }

    // initialize header
    can_header.StdId = 0xAA;
    can_header.ExtId = 0xAA;
    can_header.IDE = CAN_ID_STD;
    can_header.RTR = CAN_RTR_DATA;
    can_header.DLC = 8;
    can_header.TransmitGlobalTime = DISABLE;
}

void can_send_electric(uint32_t voltage, int32_t current)
{
    uint8_t data[8];
    
    data[0] = BCP_MSG_ELECTRIC;

    struct bcp_msg_electric* el = (struct bcp_msg_electric*)&data[1];

    el->timestamp = HAL_GetTick() % MAX_TIMESTAMP;
    el->voltage = voltage;
    el->current = convert_to_14bit(current);
    el->faults = 0;
    el->seq_id = electric_seq_id++;

    _send_can(data);
}

void can_send_motion(uint32_t tot_pulses)
{
    uint8_t data[8];
    
    data[0] = BCP_MSG_MOTION;

    struct bcp_msg_motion* m = (struct bcp_msg_motion*)&data[1];

    m->timestamp = HAL_GetTick() % MAX_TIMESTAMP;
    m->tot_pulses = tot_pulses;
    m->seq_id = motion_seq_id++;
    m->reserved = 0;

    _send_can(data);
}

void can_send_temp(int32_t moto_t, int32_t drv_t, int32_t batt_t)
{
    uint8_t data[8];
    
    data[0] = BCP_MSG_SENS_BLK1;

    struct bcp_msg_sens_blk1* blk = (struct bcp_msg_sens_blk1*)&data[1];

    blk->moto_t = convert_to_9bit(moto_t);
    blk->drv_t = convert_to_9bit(drv_t);
    blk->batt_t = convert_to_9bit(batt_t);

    _send_can(data);
}