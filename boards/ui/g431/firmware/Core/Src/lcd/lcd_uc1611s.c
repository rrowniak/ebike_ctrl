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

static void lcd_spi_tx(const uint8_t *buf, uint16_t len)
{
    HAL_SPI_Transmit(spi_handle, (uint8_t *)buf, len, HAL_MAX_DELAY);
}

void lcd_uc1611s_flush(void)
{
    /* Write the framebuffer page by page (each page = 240 columns of one
       8-row band).  The controller's CS pin is a bus-cycle reset, so the
       column/page address and the frame data for each page must share ONE
       CS-low transaction (toggle CD between command and data), otherwise
       intervening CS-highs can drop the address setup and lose whole pages. */
    uint8_t addr[4];
    for (int page = 0; page < LCD_PAGES; page++) {
        /* Open a single bus cycle for this page: 4 address bytes + data.
           Set Page Address is a two-byte command (PA MSB 0x70 then PA LSB
           0x60|page); the high byte must be sent or the page register is
           not latched per the UC1611S command definition. */
        lcd_cs_low();

        addr[0] = 0x00;                       /* column address low byte = 0 */
        addr[1] = 0x10;                       /* column address high byte = 0 */
        addr[2] = 0x70;                       /* page address high byte (0) */
        addr[3] = 0x60 | page;                /* page address low byte */
        lcd_cd_command();
        lcd_spi_tx(addr, 4);

        lcd_cd_data();
        lcd_spi_tx(&fb[page * LCD_WIDTH], LCD_WIDTH);

        lcd_cs_high();
    }
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
