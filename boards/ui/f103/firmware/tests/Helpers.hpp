#ifndef __HELPERS_HPP__
#define __HELPERS_HPP__

#include <stm32_puppet.hpp>
#include <hd44780_puppet.hpp>
#include <bike_can_protocol.h>

extern uint32_t electric_seq_id;
extern uint32_t motion_seq_id ;

void dump_lcd()
{
    std::cout << hd44780_get_frame() << '\n';
    std::cout << hd44780_get_line1() << '\n';
    std::cout << hd44780_get_line2() << '\n';
    std::cout << hd44780_get_frame() << std::endl;
}

CanMessage BuildElectricMsg(uint32_t voltage, int32_t current)
{
    CanMessage msg;

    msg.data[0] = BCP_MSG_ELECTRIC;

    struct bcp_msg_electric* el = (struct bcp_msg_electric*)&msg.data[1];

    el->timestamp = HAL_GetTick() % MAX_TIMESTAMP;
    el->voltage = voltage;
    el->current = convert_to_14bit(current);
    el->faults = 0;
    el->seq_id = electric_seq_id++;

    return msg;
}

CanMessage BuildMotionMsg(uint32_t tot_pulses)
{
    CanMessage msg;

    msg.data[0] = BCP_MSG_MOTION;

    struct bcp_msg_motion* m = (struct bcp_msg_motion*)&msg.data[1];

    m->timestamp = HAL_GetTick() % MAX_TIMESTAMP;
    m->tot_pulses = tot_pulses;
    m->seq_id = motion_seq_id++;
    m->reserved = 0;

    return msg;
}

CanMessage BuildTempMsg(int32_t moto_t, int32_t drv_t, int32_t batt_t)
{
    CanMessage msg;

    msg.data[0] = BCP_MSG_SENS_BLK1;

    struct bcp_msg_sens_blk1* blk = (struct bcp_msg_sens_blk1*)&msg.data[1];

    blk->moto_t = convert_to_9bit(moto_t);
    blk->drv_t = convert_to_9bit(drv_t);
    blk->batt_t = convert_to_9bit(batt_t);

    return msg;
}

#endif