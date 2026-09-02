#include "screens.h"
#include "lcd/lcd_uc1611s_graphics.h"
#include "lcd/lcd_font_small.h"
#include "lcd/lcd_font_medium.h"
#include "lcd/lcd_font_large.h"
#include <stdio.h>

/* Out-of-font glyph marker rendered as a degree sign (0x7F is outside the
   ASCII 0x20..0x7E range covered by the fonts). */
#define DEG     0x7F
#define DEG_STR "\x7f"

/* Screen geometry */
#define COL_DIV_X 160
#define BAR_Y     10

static const uint8_t icon_lights[8] = {
    0x18, 0x18, 0x3C, 0x7E, 0x7E, 0x7E, 0x3C, 0x3C
};

static const uint8_t icon_fault[8] = {
    0x10, 0x38, 0x38, 0x7C, 0x7C, 0xFE, 0xFE, 0x7C
};

static const uint8_t icon_offline[8] = {
    0x00, 0x7E, 0x89, 0x91, 0xA1, 0xC1, 0x7E, 0x00
};

static const uint8_t icon_alarm[8] = {
    0x08, 0x08, 0x18, 0x18, 0x18, 0x3C, 0x3C, 0x3C
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
    for (int row = 0; row < 8; row++) {
        uint8_t bits = bmp[row];
        for (int col = 0; col < 8; col++)
            if (bits & (1U << (7 - col)))
                lcd_pixel(x + col, y + row, 1);
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
    text_deg(72, 92, "CAWAhkmV", font_large);
}

void main_screen_p1(const screens_data_t *d)
{
    char buf[16];
    lcd_fill(0);

    /* ---- Status bar ---- */
    int ix = 4;
    if (d->lights_on) { draw_icon(ix, 1, icon_lights); ix += 10; }
    if (d->fault)     { draw_icon(ix, 1, icon_fault);  ix += 10; }
    if (d->offline)   { draw_icon(ix, 1, icon_offline); ix += 10; }
    if (d->alarm)     { draw_icon(ix, 1, icon_alarm);  ix += 10; }

    snprintf(buf, sizeof buf, "%d/%d", d->page, d->pages);
    lcd_text(LCD_WIDTH - 6 - lcd_text_width(buf, font_small), 1, buf, font_small);

    lcd_hline(0, BAR_Y, LCD_WIDTH, 1);
    lcd_vline(COL_DIV_X, BAR_Y + 1, LCD_HEIGHT - BAR_Y - 1, 1);

    /* ---- Left column: riding data ---- */
    snprintf(buf, sizeof buf, "%d.%d", d->speed_x10 / 10, d->speed_x10 % 10);
    text_deg_c(COL_DIV_X / 2, 16, buf, font_large);
    text_deg_c(COL_DIV_X / 2, 58, "km/h", font_small);

    lcd_hline(20, 100, 120, 1);

    snprintf(buf, sizeof buf, "%u W", d->watts);
    text_deg(38, 105, buf, font_medium);
    lcd_vline(89, 110, 10, 1);
    snprintf(buf, sizeof buf, "%d" DEG_STR "C", d->temp_c);
    text_deg(99, 114, buf, font_small);

    /* ---- Right column: battery block ---- */
    /* Battery icon centered in right column (x 160..240) */
    int bx = 167, by = 16;
    lcd_rect(bx, by, 22, 15, 1);
    lcd_rect(bx + 22, by + 4, 4, 7, 1);
    int fill = 22 * d->soc / 100;
    if (fill > 0)
        lcd_fill_rect(bx + 1, by + 1, fill, 13, 1);

    snprintf(buf, sizeof buf, "%u%%", d->soc);
    text_deg_c(200, 34, buf, font_large);

    int cells = 10;
    int cw = 6;
    int bar_x = 200 - (cells * cw + cells - 1) / 2;
    lcd_rect(bar_x - 2, 74, cells * cw + cells - 1 + 4, 12, 1);
    int cells_filled = cells * d->soc / 100;
    for (int i = 0; i < cells; i++) {
        int cx = bar_x + i * (cw + 1);
        if (i < cells_filled)
            lcd_fill_rect(cx, 77, cw, 6, 1);
        else
            lcd_rect(cx, 77, cw, 6, 1);
    }

    snprintf(buf, sizeof buf, "%u.%u V", d->voltage_x10 / 10, d->voltage_x10 % 10);
    text_deg(164, 100, buf, font_small);
    snprintf(buf, sizeof buf, "%u km", d->range_km);
    text_deg(205, 100, buf, font_small);
}