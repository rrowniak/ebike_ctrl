#include "screens.h"
#include "lcd/lcd_uc1611s_graphics.h"
#include "lcd/lcd_font_small.h"
#include "lcd/lcd_font_medium.h"
#include "lcd/lcd_font_large.h"
#include "lcd/lcd_font_xlarge.h"
#include <stdio.h>

/* Out-of-font glyph marker rendered as a degree sign (0x7F is outside the
   ASCII 0x20..0x7E range covered by the fonts). */
#define DEG     0x7F
#define DEG_STR "\x7f"

/* Screen geometry */
#define COL_DIV_X 160
#define BAR_Y     18

/* 16x16 status icons (bit 7 = leftmost column). */
static const uint8_t icon_lights[32] = {
    0x01,0x80, 0x01,0x80, 0x03,0xC0, 0x07,0xE0,
    0x0F,0xF0, 0x1F,0xF8, 0x1F,0xF8, 0x0F,0xF0,
    0x0F,0xF0, 0x0F,0xF0, 0x1F,0xF8, 0x1F,0xF8,
    0x1F,0xF8, 0x0F,0xF0, 0x07,0xE0, 0x03,0xC0,
};

static const uint8_t icon_fault[32] = {
    0x01,0x80, 0x01,0x80, 0x03,0xC0, 0x03,0xC0,
    0x07,0xE0, 0x07,0xE0, 0x0F,0xF0, 0x0F,0xF0,
    0x1F,0xF8, 0x0F,0xF0, 0x07,0xE0, 0x03,0xC0,
    0x1F,0xF8, 0x0F,0xF0, 0x07,0xE0, 0x03,0xC0,
};

static const uint8_t icon_offline[32] = {
    0x00,0x00, 0x7F,0xFE, 0xFF,0xFF, 0xE0,0x07,
    0xC8,0x13, 0x9B,0xD9, 0x9F,0xF9, 0x9F,0xF9,
    0x9F,0xF9, 0x9F,0xF9, 0x9B,0xD9, 0xC8,0x13,
    0xE0,0x07, 0xFF,0xFF, 0x7F,0xFE, 0x00,0x00,
};

static const uint8_t icon_alarm[32] = {
    0x00,0x00, 0x00,0x00, 0x18,0x18, 0x18,0x18,
    0x18,0x18, 0x18,0x18, 0x3C,0x3C, 0x3C,0x3C,
    0x3C,0x3C, 0x5A,0x5A, 0x7E,0x7E, 0x3C,0x3C,
    0x00,0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00,
};

/* Render a string, drawing the DEG marker as a small ring (degree sign) in
   the right-hand upper corner of its glyph cell.  Non-ASCII markers advance
   by one full glyph width so the rest of the text stays aligned. */
static void text_deg(int x, int y, const char *str, lcd_font_t font)
{
    const unsigned char *p = (const unsigned char *)str;

    while (*p) {
        if (*p == DEG) {
            int box, cy;
            if (font.height >= 32)      { box = 8; cy = y + 6; }
            else if (font.height >= 12) { box = 5; cy = y + 3; }
            else                        { box = 3; cy = y + 1; }
            lcd_rect(x, cy, box, box, 1);
            if (box >= 5)
                lcd_fill_rect(x + 2, cy + 2, box - 4, box - 4, 0);
            x += font.width;
            p++;
            continue;
        }

        if (*p >= font.first_char && *p <= font.last_char) {
            char glyph[2] = { (char)*p, '\0' };
            lcd_text(x, y, glyph, font);
        }
        x += font.width;
        p++;
    }
}

static int text_deg_width(const char *str, lcd_font_t font)
{
    int n = 0;
    while (*str++) n++;
    return n * font.width;
}

/* Center a string horizontally on cx. */
static void text_deg_c(int cx, int y, const char *str, lcd_font_t font)
{
    text_deg(cx - text_deg_width(str, font) / 2, y, str, font);
}

static void draw_icon(int x, int y, const uint8_t *bmp)
{
    for (int row = 0; row < 16; row++) {
        uint8_t hi = bmp[row * 2];
        uint8_t lo = bmp[row * 2 + 1];
        for (int col = 0; col < 16; col++) {
            uint8_t bit = (col < 8) ? (hi & (1U << (7 - col))) : (lo & (1U << (15 - col)));
            if (bit)
                lcd_pixel(x + col, y + row, 1);
        }
    }
}

void test_screen(void)
{
    lcd_fill(0);

    text_deg_c(120, 4, "TEST SCREEN", font_medium);
    lcd_hline(16, 21, 208, 1);

    text_deg(48, 26, "0123456789.+-/%" DEG_STR "CAWAhkmV", font_small);
    text_deg(24, 36, "0123456789.+-/%" DEG_STR "CAWAhkmV", font_medium);
    text_deg(24, 54, "0123456789.+-/%" DEG_STR, font_large);
    text_deg(24, 76, "25.3-", font_xlarge);
    text_deg(72, 92, "CAWAhkmV", font_large);
}

void main_screen_p1(const screens_data_t *d)
{
    char buf[16];
    lcd_fill(0);

    /* ---- Status bar ---- */
    int ix = 2;
    if (d->lights_on) { draw_icon(ix, 1, icon_lights); ix += 18; }
    if (d->fault)     { draw_icon(ix, 1, icon_fault);  ix += 18; }
    if (d->offline)   { draw_icon(ix, 1, icon_offline); ix += 18; }
    if (d->alarm)     { draw_icon(ix, 1, icon_alarm);  ix += 18; }

    snprintf(buf, sizeof buf, "%d/%d", d->page, d->pages);
    lcd_text(LCD_WIDTH - 6 - lcd_text_width(buf, font_medium), 5, buf, font_medium);

    lcd_hline(0, BAR_Y, LCD_WIDTH, 1);
    lcd_vline(COL_DIV_X, BAR_Y + 1, LCD_HEIGHT - BAR_Y - 1, 1);

    /* ---- Left column: riding data ---- */
    snprintf(buf, sizeof buf, "%d.%d", d->speed_x10 / 10, d->speed_x10 % 10);
    text_deg_c(COL_DIV_X / 2, 20, buf, font_xlarge);
    text_deg_c(COL_DIV_X / 2, 72, "km/h", font_medium);

    lcd_hline(20, 92, 124, 1);

    snprintf(buf, sizeof buf, "%u W", d->watts);
    text_deg(24, 98, buf, font_medium);
    lcd_vline(85, 100, 12, 1);
    snprintf(buf, sizeof buf, "%d" DEG_STR "C", d->temp_c);
    text_deg(96, 98, buf, font_medium);

    /* ---- Right column: battery block ---- */
    /* Single battery visualization: fill-style icon + SOC% + V + range. */
    const int bx = 190, by = 26;               /* battery outline, centered ~x202 */
    lcd_rect(bx, by, 24, 15, 1);
    lcd_rect(bx - 3, by + 4, 3, 7, 1);
    int fill = 22 * d->soc / 100;
    if (fill > 0)
        lcd_fill_rect(bx + 1, by + 1, fill, 13, 1);

    snprintf(buf, sizeof buf, "%u", d->soc);   /* SOC number in xlarge */
    text_deg(202 - lcd_text_width(buf, font_xlarge) / 2, 48, buf, font_xlarge);
    lcd_text(202 + lcd_text_width(buf, font_xlarge) / 2 + 2, 48, "%", font_large);

    snprintf(buf, sizeof buf, "%u.%uV", d->voltage_x10 / 10, d->voltage_x10 % 10);
    text_deg_c(COL_DIV_X + (LCD_WIDTH - COL_DIV_X) / 2, 100, buf, font_medium);
    snprintf(buf, sizeof buf, "%ukm", d->range_km);
    text_deg_c(COL_DIV_X + (LCD_WIDTH - COL_DIV_X) / 2, 118, buf, font_medium);
}