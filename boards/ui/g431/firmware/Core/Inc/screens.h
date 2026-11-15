#ifndef SCREENS_H
#define SCREENS_H

#include <stdint.h>

typedef struct {
    uint16_t speed_x10;      /* km/h * 10 */
    uint16_t watts;          /* instant power */
    int8_t   temp_c;         /* ambient temperature */
    uint8_t  soc;            /* state of charge, % */
    uint16_t voltage_x10;    /* battery voltage * 10 */
    uint16_t range_km;       /* estimated range */
    uint32_t trip_m;         /* trip distance, meters */
    uint32_t trip_wh_x10;    /* trip energy, 0.1 Wh units */
    uint32_t trip_move_s;    /* trip moving time, seconds */
    uint8_t  lights_on;      /* light icon */
    uint8_t  fault;          /* active fault icon */
    uint8_t  offline;        /* no motherboard signal */
    uint8_t  alarm;          /* temperature alarm */
    uint8_t  page;           /* main screen sub-page number */
    uint8_t  pages;          /* total main screen sub-pages */
} screens_data_t;

void test_screen(void);
void main_screen_p1(const screens_data_t *data);

#endif /* SCREENS_H */