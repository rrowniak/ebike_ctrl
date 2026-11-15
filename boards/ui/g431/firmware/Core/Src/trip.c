#include "trip.h"

/* Speed at which the bike counts as moving: 1.0 km/h. */
#define MOVE_SPEED_X10   10

static struct {
    uint64_t dist_u36;       /* distance in units of 1/36 mm */
    uint64_t energy_mws;     /* energy in milli-watt-seconds (Wh_x10 * 360000) */
    uint32_t move_ms;        /* moving time with sub-second resolution */
} s_trip;

void trip_reset(void)
{
    s_trip.dist_u36   = 0;
    s_trip.energy_mws = 0;
    s_trip.move_ms    = 0;
}

void trip_set(uint32_t dist_m, uint32_t wh_x10, uint32_t move_s)
{
    s_trip.dist_u36   = (uint64_t)dist_m * 36000;    /* 1000 mm * 36 units/mm */
    s_trip.energy_mws = (uint64_t)wh_x10 * 360000;
    s_trip.move_ms    = (uint32_t)((uint64_t)move_s * 1000);
}

void trip_update(uint32_t dt_ms, uint16_t speed_x10, uint16_t watts)
{
    if (dt_ms == 0)
        return;

    /* speed_x10 is 0.1 km/h, so speed_x10 / 36 is m/s.  Accumulating
       speed_x10 * dt_ms therefore counts 1/36 mm units of distance; keeping
       the full 64 bits means no distance is lost to rounding. */
    s_trip.dist_u36 += (uint64_t)speed_x10 * dt_ms;

    s_trip.energy_mws += (uint64_t)watts * dt_ms;

    if (speed_x10 >= MOVE_SPEED_X10)
        s_trip.move_ms += dt_ms;
}

uint32_t trip_distance_m(void)
{
    return (uint32_t)(s_trip.dist_u36 / 36000);   /* 36 units per mm, 1000 mm per m */
}

uint32_t trip_energy_wh_x10(void)
{
    return (uint32_t)(s_trip.energy_mws / 360000);
}

uint32_t trip_move_time_s(void)
{
    return s_trip.move_ms / 1000;
}
