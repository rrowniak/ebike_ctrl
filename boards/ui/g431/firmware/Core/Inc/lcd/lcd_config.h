#ifndef LCD_CONFIG_H
#define LCD_CONFIG_H

#include "main.h"
#include <stdint.h>

#define LCD_WIDTH       240
#define LCD_HEIGHT      128
#define LCD_FB_SIZE     (LCD_WIDTH * LCD_HEIGHT / 8)

#define LCD_COL_MAX     239
#define LCD_PAGE_MAX    15
#define LCD_PAGES       (LCD_HEIGHT / 8)

#define LCD_CMD_COL_HIGH(n)     (0x10 | ((n) >> 4))
#define LCD_CMD_COL_LOW(n)      ((n) & 0x0F)
#define LCD_CMD_PAGE(n)         (0x60 | ((n) & 0x0F))

#define LCD_FONT_COLUMN_MAJOR 0
#define LCD_FONT_ROW_MAJOR    1

typedef struct {
    const uint8_t *data;
    uint8_t width;
    uint8_t height;
    uint8_t first_char;
    uint8_t last_char;
    uint8_t bytes_per_char;
    uint8_t row_major;
} lcd_font_t;

#endif /* LCD_CONFIG_H */
