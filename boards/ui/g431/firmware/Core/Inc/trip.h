#ifndef TRIP_H
#define TRIP_H

#include <stdint.h>

/* Trip meters: everything accumulated since the last trip_reset().
   trip_update() must be called regularly (any dt) with the current speed and
   power; the module integrates them into distance, energy and moving time.
   trip_set() overwrites the meters with given values, e.g. when restoring a
   saved trip or to exercise the display with worst-case readings. */

void     trip_reset(void);
void     trip_set(uint32_t dist_m, uint32_t wh_x10, uint32_t move_s);
void     trip_update(uint32_t dt_ms, uint16_t speed_x10, uint16_t watts);
uint32_t trip_distance_m(void);      /* meters */
uint32_t trip_energy_wh_x10(void);   /* 0.1 Wh units */
uint32_t trip_move_time_s(void);     /* seconds spent moving */

#endif /* TRIP_H */
