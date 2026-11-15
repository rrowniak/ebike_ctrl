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

/* Status bar: status icons on the left, trip meters right after them,
   temperature on the right.  Icons are usually absent, so the meters take the
   left edge of the bar and only shift right when something must be shown. */
#define ICON_X0       2
#define ICON_PITCH    18
#define BAR_TEXT_Y    1                              /* 16px icons/text top row */
#define TEMP_MARGIN   6                              /* screen margin right of temp */
#define TRIP_GAP_MIN  4                              /* never crowd the equal gaps */

/* 16x16 status icons (bit 7 = leftmost column). */
static const uint8_t icon_lights[32] = {
    0x03,0xC0, 0x0F,0xC0, 0x1F,0xC0, 0x3F,0xC8,
    0x7F,0xCC, 0xFF,0xE7, 0xFF,0xE1, 0xFF,0xE8,
    0xFF,0xEC, 0xFF,0xE7, 0xFF,0xE1, 0x7F,0xC8,
    0x3F,0xCC, 0x1F,0xC7, 0x0F,0xC1, 0x03,0xC0,
};

static const uint8_t icon_fault[32] = {
    0x00,0x00, 0x00,0x80, 0x01,0x40, 0x01,0x40,
    0x02,0xA0, 0x02,0xA0, 0x04,0x90, 0x08,0x88,
    0x08,0x88, 0x10,0x84, 0x10,0x04, 0x20,0x82,
    0x20,0x02, 0x7F,0xFF, 0x00,0x00, 0x00,0x00,
};

static const uint8_t icon_offline[32] = {
    0x00,0x33, 0x00,0xFF, 0x03,0xCE, 0x03,0x86,
    0x01,0xC3, 0x01,0xE3, 0x23,0x36, 0x30,0x3E,
    0x78,0x7C, 0x6C,0x4C, 0xC6,0x00, 0xC3,0x00,
    0x61,0xC0, 0x73,0xC0, 0xFF,0x00, 0xCC,0x00,
};

static const uint8_t icon_alarm[32] = {
    0x00,0x00, 0x01,0x80, 0x01,0x80, 0x01,0xE0,
    0x01,0x80, 0x01,0xE0, 0x01,0x80, 0x01,0xE0,
    0x01,0xC0, 0x01,0x80, 0xE7,0xFE, 0x3A,0xBA,
    0xC3,0x98, 0xFF,0xFF, 0x31,0x8C, 0x00,0x00,
};

/* Draw a small ring (circle outline) centred at (cx, cy) with radius r.
   Uses the Bresenham midpoint circle algorithm for a smooth circle. */
static void draw_ring(int cx, int cy, int r)
{
    if (r <= 0) { lcd_pixel(cx, cy, 1); return; }
    int x = 0, y = r, d = 3 - 2 * r;
    while (y >= x) {
        lcd_pixel(cx + x, cy + y, 1);
        lcd_pixel(cx - x, cy + y, 1);
        lcd_pixel(cx + x, cy - y, 1);
        lcd_pixel(cx - x, cy - y, 1);
        lcd_pixel(cx + y, cy + x, 1);
        lcd_pixel(cx - y, cy + x, 1);
        lcd_pixel(cx + y, cy - x, 1);
        lcd_pixel(cx - y, cy - x, 1);
        if (d < 0) d += 4 * x + 6;
        else { d += 4 * (x - y) + 10; y--; }
        x++;
    }
}

/* Render a string, drawing the DEG marker as a small ring (degree sign) in
   the right-hand upper corner of its glyph cell.  Non-ASCII markers advance
   by one full glyph width so the rest of the text stays aligned. */
static void text_deg(int x, int y, const char *str, lcd_font_t font)
{
    const unsigned char *p = (const unsigned char *)str;

    while (*p) {
        if (*p == DEG) {
            int cx, cy, r;
            if (font.height >= 32)      { cx = x + 7; cy = y + 5; r = 4; }
            else if (font.height >= 12) { cx = x + 3; cy = y + 2; r = 2; }
            else                        { cx = x + 2; cy = y + 1; r = 2; }
            draw_ring(cx, cy, r);
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

/* Moving time as "1h23m", or plain minutes below an hour. */
static void fmt_move_time(char *buf, size_t n, uint32_t s)
{
    if (s >= 3600)
        snprintf(buf, n, "%uh%02um", (unsigned)(s / 3600), (unsigned)((s / 60) % 60));
    else
        snprintf(buf, n, "%um", (unsigned)(s / 60));
}

/* Trip meters in the status bar: distance | moving time.  They are laid out
   between the status icons (x) and the temperature (x_end), sharing the free
   space equally: the gap after the icons, the gap between the two fields and
   the gap before the temperature all come out the same size.  The distance is
   expected to stay below "1000.0km"; the worst expected pair, "999.9km" +
   "12h34m", needs 104px and fits even with all four icons showing.  If the
   fields ever stop fitting with non-crowded gaps, the time is dropped and the
   distance is centered between the icons and the temperature instead. */
static void draw_trip_meters(int x, int x_end, const screens_data_t *d)
{
    char str[2][12];
    int  w[2], n, slack, gap, gap3[3], xf;

    snprintf(str[0], sizeof str[0], "%u.%ukm",
             (unsigned)(d->trip_m / 1000), (unsigned)(d->trip_m / 100) % 10);
    fmt_move_time(str[1], sizeof str[1], d->trip_move_s);
    for (int i = 0; i < 2; i++)
        w[i] = lcd_text_width(str[i], font_medium);

    /* Keep as many fields as fit without crowding the gaps. */
    slack = x_end - x;
    for (n = 2; n > 0; n--) {
        int s = slack;
        for (int i = 0; i < n; i++)
            s -= w[i];
        if (s >= (n + 1) * TRIP_GAP_MIN) {
            slack = s;
            break;
        }
        slack = 0;
    }
    if (n == 0)
        return;

    /* Split the slack into n+1 gaps, giving the odd pixels to the left ones. */
    gap = slack / (n + 1);
    for (int i = 0; i <= n; i++)
        gap3[i] = gap + (i < slack % (n + 1) ? 1 : 0);

    xf = x + gap3[0];
    for (int i = 0; i < n; i++) {
        lcd_text(xf, BAR_TEXT_Y, str[i], font_medium);
        if (i + 1 < n)
            lcd_vline(xf + w[i] + gap3[i + 1] / 2, BAR_TEXT_Y + 2, 12, 1);
        xf += w[i] + gap3[i + 1];
    }
}

void main_screen_p1(const screens_data_t *d)
{
    char buf[16];
    lcd_fill(0);

    /* ---- Status bar ---- */
    int ix = ICON_X0;
    if (d->lights_on) { draw_icon(ix, BAR_TEXT_Y, icon_lights); ix += ICON_PITCH; }

    /* Fault icon blinks (~1 Hz) to attract attention.  Its slot is always
       reserved so the icons to its right don't shift during the blink. */
    if (d->fault) {
#ifdef USE_HAL_DRIVER
        if ((HAL_GetTick() / 500) & 1)
            draw_icon(ix, BAR_TEXT_Y, icon_fault);
#else
        draw_icon(ix, BAR_TEXT_Y, icon_fault);
#endif
        ix += ICON_PITCH;
    }
    if (d->offline)   { draw_icon(ix, BAR_TEXT_Y, icon_offline); ix += ICON_PITCH; }
    if (d->alarm)     { draw_icon(ix, BAR_TEXT_Y, icon_alarm);  ix += ICON_PITCH; }

    /* Temperature is measured first: it sets the space the trip meters get,
       and it grows to the left for negative two-digit readings. */
    snprintf(buf, sizeof buf, "%d" DEG_STR "C", d->temp_c);
    int temp_x = LCD_WIDTH - TEMP_MARGIN - text_deg_width(buf, font_medium);
    text_deg(temp_x, BAR_TEXT_Y, buf, font_medium);

    draw_trip_meters(ix, temp_x, d);

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
    snprintf(buf, sizeof buf, "%ukm", d->range_km);
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
    text_deg_c(COL_DIV_X + (LCD_WIDTH - COL_DIV_X) / 2, 110, buf, font_medium);
}
