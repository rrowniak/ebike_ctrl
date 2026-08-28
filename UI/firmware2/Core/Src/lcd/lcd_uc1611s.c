#include "lcd/lcd_uc1611s.h"

static SPI_HandleTypeDef *spi_handle;

static uint8_t fb[LCD_FB_SIZE];

static void lcd_cs_low(void)
{
    HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET);
}

static void lcd_cs_high(void)
{
    HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET);
}

static void lcd_cd_command(void)
{
    HAL_GPIO_WritePin(LCD_CD_GPIO_Port, LCD_CD_Pin, GPIO_PIN_RESET);
}

static void lcd_cd_data(void)
{
    HAL_GPIO_WritePin(LCD_CD_GPIO_Port, LCD_CD_Pin, GPIO_PIN_SET);
}

void lcd_uc1611s_command(uint8_t cmd)
{
    lcd_cs_low();
    lcd_cd_command();
    HAL_SPI_Transmit(spi_handle, &cmd, 1, HAL_MAX_DELAY);
    lcd_cs_high();
}

void lcd_uc1611s_data(uint8_t data)
{
    lcd_cs_low();
    lcd_cd_data();
    HAL_SPI_Transmit(spi_handle, &data, 1, HAL_MAX_DELAY);
    lcd_cs_high();
}

void lcd_uc1611s_init(SPI_HandleTypeDef *hspi)
{
    spi_handle = hspi;

    static const uint8_t init_cmds[] = {
        0xF1, 0x7F,
        0xF2, 0x00,
        0xF3, 0x7F,
        0x81, 0x8F,
        0xC0, 0x02,
        0xA3,
        0x25,
        0xA9,
        0xD1,
        0x89,
        0x00, 0x10,
        0x60, 0x70,
    };

    for (unsigned i = 0; i < sizeof(init_cmds); i++)
        lcd_uc1611s_command(init_cmds[i]);
}

void lcd_uc1611s_flush(void)
{
    lcd_cs_low();
    lcd_cd_data();
    HAL_SPI_Transmit(spi_handle, fb, LCD_FB_SIZE, HAL_MAX_DELAY);
    lcd_cs_high();
}

void lcd_uc1611s_display_on(void)
{
    lcd_uc1611s_command(0xA9);
}

void lcd_uc1611s_display_off(void)
{
    lcd_uc1611s_command(0xA8);
}

void lcd_uc1611s_set_contrast(uint8_t val)
{
    lcd_uc1611s_command(0x81);
    lcd_uc1611s_command(val);
}

uint8_t *lcd_uc1611s_get_fb(void)
{
    return fb;
}
