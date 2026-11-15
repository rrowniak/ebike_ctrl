#ifndef LCD_UC1611S_H
#define LCD_UC1611S_H

#include "lcd_config.h"

void lcd_uc1611s_init(SPI_HandleTypeDef *hspi);
void lcd_uc1611s_command(uint8_t cmd);
void lcd_uc1611s_data(uint8_t data);
void lcd_uc1611s_flush(void);
void lcd_uc1611s_display_on(void);
void lcd_uc1611s_display_off(void);
void lcd_uc1611s_set_contrast(uint8_t val);
uint8_t *lcd_uc1611s_get_fb(void);

#endif /* LCD_UC1611S_H */
