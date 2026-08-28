#ifndef LCD_UC1611S_GRAPHICS_H
#define LCD_UC1611S_GRAPHICS_H

#include "lcd_config.h"

void lcd_init(void);
void lcd_on(void);
void lcd_off(void);
void lcd_clear(void);
void lcd_set_contrast(uint8_t val);

void lcd_pixel(int x, int y, uint8_t color);
uint8_t lcd_get_pixel(int x, int y);
void lcd_fill(uint8_t color);

void lcd_hline(int x, int y, int w, uint8_t color);
void lcd_vline(int x, int y, int h, uint8_t color);
void lcd_rect(int x, int y, int w, int h, uint8_t color);
void lcd_fill_rect(int x, int y, int w, int h, uint8_t color);

void lcd_text(int x, int y, const char *str, lcd_font_t font);
int  lcd_text_width(const char *str, lcd_font_t font);
int  lcd_text_height(lcd_font_t font);

void lcd_flush(void);

#endif /* LCD_UC1611S_GRAPHICS_H */
