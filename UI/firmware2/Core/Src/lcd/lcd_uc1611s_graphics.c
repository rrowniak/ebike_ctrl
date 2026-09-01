#include "lcd/lcd_uc1611s_graphics.h"
#include "lcd/lcd_uc1611s.h"
#include "lcd/lcd_font_small.h"
#include <string.h>

void lcd_init(void)
{
    lcd_uc1611s_init(&hspi1);
    lcd_clear();
}

void lcd_on(void)
{
    lcd_uc1611s_display_on();
}

void lcd_off(void)
{
    lcd_uc1611s_display_off();
}

void lcd_clear(void)
{
    memset(lcd_uc1611s_get_fb(), 0, LCD_FB_SIZE);
}

void lcd_set_contrast(uint8_t val)
{
    lcd_uc1611s_set_contrast(val);
}

void lcd_pixel(int x, int y, uint8_t color)
{
    if (x < 0 || x >= LCD_WIDTH || y < 0 || y >= LCD_HEIGHT)
        return;

    uint8_t *fb = lcd_uc1611s_get_fb();
    unsigned page = (unsigned)y / 8;
    unsigned bit  = (unsigned)y % 8;
    unsigned idx  = page * LCD_WIDTH + (unsigned)x;

    if (color)
        fb[idx] |= (1U << bit);
    else
        fb[idx] &= ~(1U << bit);
}

uint8_t lcd_get_pixel(int x, int y)
{
    if (x < 0 || x >= LCD_WIDTH || y < 0 || y >= LCD_HEIGHT)
        return 0;

    const uint8_t *fb = lcd_uc1611s_get_fb();
    unsigned page = (unsigned)y / 8;
    unsigned bit  = (unsigned)y % 8;
    unsigned idx  = page * LCD_WIDTH + (unsigned)x;

    return (fb[idx] >> bit) & 1;
}

void lcd_fill(uint8_t color)
{
    memset(lcd_uc1611s_get_fb(), color ? 0xFF : 0x00, LCD_FB_SIZE);
}

void lcd_hline(int x, int y, int w, uint8_t color)
{
    for (int i = 0; i < w; i++)
        lcd_pixel(x + i, y, color);
}

void lcd_vline(int x, int y, int h, uint8_t color)
{
    for (int i = 0; i < h; i++)
        lcd_pixel(x, y + i, color);
}

void lcd_rect(int x, int y, int w, int h, uint8_t color)
{
    lcd_hline(x, y, w, color);
    lcd_hline(x, y + h - 1, w, color);
    lcd_vline(x, y, h, color);
    lcd_vline(x + w - 1, y, h, color);
}

void lcd_fill_rect(int x, int y, int w, int h, uint8_t color)
{
    for (int j = 0; j < h; j++)
        lcd_hline(x, y + j, w, color);
}

void lcd_text(int x, int y, const char *str, lcd_font_t font)
{
    while (*str) {
        if (*str < font.first_char || *str > font.last_char) {
            x += font.width;
            str++;
            continue;
        }

        unsigned char_idx = *str - font.first_char;
        const uint8_t *glyph = &font.data[char_idx * font.bytes_per_char];

        if (font.row_major) {
            for (int row = 0; row < font.height; row++) {
                uint8_t bits = glyph[row];
                for (int col = 0; col < 8; col++) {
                    if (bits & (1U << (7 - col)))
                        lcd_pixel(x + col, y + row, 1);
                }
            }
        } else {
            int planes = font.height / 8;
            for (int col = 0; col < font.width; col++) {
                for (int plane = 0; plane < planes; plane++) {
                    uint8_t bits = glyph[plane * font.width + col];
                    for (int row = 0; row < 8; row++) {
                        if (bits & (1U << row))
                            lcd_pixel(x + col, y + plane * 8 + row, 1);
                    }
                }
            }
        }

        x += font.width;
        str++;
    }
}

int lcd_text_width(const char *str, lcd_font_t font)
{
    return (int)strlen(str) * font.width;
}

int lcd_text_height(lcd_font_t font)
{
    return font.height;
}

void lcd_flush(void)
{
    lcd_uc1611s_flush();
}
