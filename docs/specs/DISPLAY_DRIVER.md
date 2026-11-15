# Display Driver Implementation Plan

> Display: EA DOGXL240W-7 (240x128 COG LCD, SPI, UC1611S controller)
> MCU: STM32G431KB (NUCLEO-G431KB)
> Location: `UI/firmware2/` (local to UI project, not in l_rr library)

---

## 1. Display Hardware

| Parameter | Value |
|-----------|-------|
| Part | EA DOGXL240W-7 (datasheet `spec/EA+DOGXL240B-7.pdf` covers full DOGXL240-7 series) |
| Resolution | 240 x 128 pixels |
| Controller | UC1611S |
| Color | Monochrome (1-bit, FSTN positive transflective) |
| Supply | 2.7-3.3 V |
| Backlight | LED, 10-150 mA (separate control) |
| Package | 40-pin SIP, 2.54 mm pitch |
| Operating temp | -20 to +70 C |

### 1.1 UC1611S Controller

- **Frame buffer**: 240 x 128 pixels, 1-bit per pixel = **3,840 bytes** (fits in G431's 32 KB RAM)
- **Internal SRAM** organization: column-major pages (8 pixels per page, 16 pages for 128 rows)
- **Contrast**: software adjustable via register 0x81, range 0-255
- **Gray levels**: supports 4-bit per pixel (16 gray shades) but we use 1-bit mode for simplicity

### 1.2 Interface Modes (from datasheet)

The EA DOGXL240W-7 supports three serial modes, selected by **BM0** and **D13** hardware pins:

| BM0 | D13 | Mode | Description |
|-----|-----|------|-------------|
| 0 | 0 | **4-wire, 8-bit SPI** | CD pin selects command/data. 8 bits per transfer. CS0 active-low. |
| 1 | 0 | **3-wire, 9-bit SPI** | CD bit sent as 1st bit after CS, followed by 8 data bits. No separate CD pin. |
| 1 | 1 | 2-wire, I2C | Bidirectional. Up to 4 displays on one bus. |

**Selected mode**: 4-wire, 8-bit SPI (BM0=0, D13=0).

Rationale:
- CubeMX already has CD pin (PB6) wired as GPIO output — directly maps to 4-wire mode
- Simplest to implement with STM32 HAL SPI — each byte is a standard SPI transfer
- Unidirectional (write-only) — no MISO pin needed
- Max clock: 8 MHz (current prescaler gives ~5.3 MHz — within spec)

### 1.3 SPI Timing (UC1611S)

From the datasheet: data is latched on the **rising edge** of SCK, clock idles **low**.

This corresponds to:
- **CPOL = 0** (clock idle low)
- **CPHA = 0** (data sampled on leading/rising edge)

In STM32 HAL terms: **SPI Mode 0** (`SPI_POLARITY_LOW`, `SPI_PHASE_1EDGE`).

The current `MX_SPI1_Init()` in `UI/firmware2/Core/Src/main.c` is already configured correctly:
```c
hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;    // CPOL=0
hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;        // CPHA=0
```

### 1.4 Pin Connections

| SPI Signal | MCU Pin | AF | Display Pin | Notes |
|------------|---------|-----|-------------|-------|
| SCK | PB3 | AF5 | 38 (SCK) | Cut SB4 to free from SWO trace |
| MOSI | PB5 | AF5 | 39 (SDA) | User LED LD2 on same pin |
| CS (GPIO) | PB4 | — | 36 (CS1) | Active LOW |
| CD (GPIO) | PB6 | — | 34 (CD) | HIGH = data, LOW = command |
| Backlight | PA7 | — | — | Active HIGH, GPIO toggle |

See `spec/UI_PIN_MAPPING.md` Section 2.1 for full details.

---

## 2. Architecture

Four layers, bottom-up:

```
+---------------------------------------------------+
|  ui.c  (application logic - reworked)             |
+---------------------------------------------------+
|  lcd_uc1611s_graphics.h  (pixel + text API)       |
|  - pixel ops, primitives, text rendering           |
+---------------------------------------------------+
|  lcd_uc1611s.h  (low-level driver)                |
|  - init commands, SPI transport, framebuffer       |
+---------------------------------------------------+
|  STM32 HAL: SPI1, GPIO (CS, CD, LIGHT)            |
+---------------------------------------------------+
```

### 2.1 Why pixel-based (not character-based)?

The old display was 16x2 characters. The new one is 240x128 pixels. A character-based
abstraction would waste most of the resolution and make future UI enhancements impossible.
The old `ui.c` will be rewritten to use the new pixel API.

---

## 3. File Structure

All files under `UI/firmware2/`:

```
Inc/
  lcd/
    lcd_config.h              // Screen dims, font config, pin defines
    lcd_uc1611s.h             // Low-level driver API
    lcd_uc1611s_graphics.h    // Graphics + text API
    lcd_font_small.h          // 6x8 bitmap font (~600 bytes)
    lcd_font_medium.h         // 8x16 bitmap font (~1.5 KB)
    lcd_font_large.h          // 12x24 bitmap font (~3 KB)
  ui.h
  system.h

Src/
  lcd/
    lcd_uc1611s.c             // SPI transport, init sequence, framebuffer flush
    lcd_uc1611s_graphics.c    // Pixel ops, primitives, text rendering
    lcd_font_small.c          // Font data arrays (or .h with static const data)
  ui.c                        // Reworked UI logic
  system.c                    // Backlight, buttons (ported from old firmware)
  main.c                      // Modified: wire lcd_init, logic, buttons
```

---

## 4. UC1611S Init Sequence

Derived from the EA DOGXL240B-7 datasheet's 6:00 (bottom view) initialization
example (pages 6-7), with driver-specific additions noted below.

**The datasheet is the oracle.** When in doubt, the EA DOGXL240B-7 datasheet
(and the referenced UC1611S controller datasheet) take precedence over this
spec and over third-party implementations (u8g2 etc.). The implementation
should be verified against these datasheets before targeting real hardware.

```c
// Set COM end = 127 (128 rows - 1)
0xF1, 0x7F

// Display line start = 0
0xF2, 0x00

// Display line end = 127
0xF3, 0x7F

// Set contrast (initial value, tunable)
0x81, 0x8F

// LCD mapping (bottom view / 6:00)
0xC0, 0x02

// Line rate (normal, 9.4 kilo-lines/s)
0xA3

// Temperature compensation (-0.10%/°C)
0x25

// Display enable (B&W mode)
0xA9

// Display pattern (B&W mode)
0xD1

// --- Driver additions (not in datasheet init example) ---

// Auto increment (column only, wrap around at end)
0x89

// Column address = 0
0x00, 0x10

// Page address = 0
0x60, 0x70
```

Commands after the separator are driver additions: auto-increment and initial
address setup are not in the datasheet's example but are required for the
framebuffer flush to work correctly (column auto-increment with wrap-around,
starting at page 0, column 0).

Note: the datasheet does **not** include software reset (`0xE2`) or internal
pump control (`0x2F`) in this init example. The EA DOGXL240 module has its own
voltage converter circuitry, so the internal pump is not needed.

### 4.1 RST Pin

No RST pin is assigned in CubeMX. The UC1611S hardware reset pin (active-low) is
**optional** — per the datasheet, when unused it should be tied to VDD.

The init sequence relies on the default reset state of the controller after
power-on. If stability issues arise on real hardware (power glitches, unreliable
init), a RST pin can be added later — it's a one-line CubeMX change and a
`LCD_RST_Pin` define in `lcd_config.h`.

---

## 5. Framebuffer

### 5.1 Layout

```c
static uint8_t fb[LCD_WIDTH * LCD_HEIGHT / 8];  // 240 x 128 / 8 = 3,840 bytes
```

Page-organized internally (UC1611S native format):
- Each "page" = 8 vertical pixels
- 240 columns x 16 pages = 240 x 128 pixels
- Byte at `fb[page * 240 + col]` stores 8 vertical pixels (bit 0 = top)

### 5.2 Flush Strategy

- `lcd_flush()`: sends entire framebuffer over SPI in one burst (~3.8 KB at 5.3 MHz = ~5.7 ms)
- Partial updates via `lcd_flush_region(x, y, w, h)` — optional optimization for later
- CS is held low for the entire flush transaction

---

## 6. Graphics API

```c
// --- Low-level (lcd_uc1611s.h) ---
void lcd_uc1611s_init(SPI_HandleTypeDef *hspi);
void lcd_uc1611s_command(uint8_t cmd);
void lcd_uc1611s_data(uint8_t data);
void lcd_uc1611s_flush(void);
void lcd_uc1611s_display_on(void);
void lcd_uc1611s_display_off(void);
void lcd_uc1611s_set_contrast(uint8_t val);

// --- High-level graphics (lcd_uc1611s_graphics.h) ---
void lcd_init(void);        // calls lcd_uc1611s_init + clears framebuffer
void lcd_on(void);
void lcd_off(void);
void lcd_clear(void);
void lcd_set_contrast(uint8_t val);

// Pixel operations
void lcd_pixel(int x, int y, uint8_t color);   // color: 0=black, 1=white
uint8_t lcd_get_pixel(int x, int y);
void lcd_fill(uint8_t color);

// Primitives
void lcd_hline(int x, int y, int w, uint8_t color);
void lcd_vline(int x, int y, int h, uint8_t color);
void lcd_rect(int x, int y, int w, int h, uint8_t color);
void lcd_fill_rect(int x, int y, int w, int h, uint8_t color);

// Text
void lcd_text(int x, int y, const char *str, lcd_font_t font);
int  lcd_text_width(const char *str, lcd_font_t font);
int  lcd_text_height(lcd_font_t font);

// Framebuffer
void lcd_flush(void);  // push framebuffer to display via SPI
```

---

## 7. Font System

### 7.1 Design

- Bitmap fonts stored as `const uint8_t[]` arrays in header files
- Each character stored column-major (matching UC1611S RAM layout for fast rendering)
- Character range: 0x20-0x7E (printable ASCII, ~95 chars)
- Three sizes:

| Name | Dimensions | Bytes/char | Total (~95 chars) | Use case |
|------|-----------|------------|---------------------|----------|
| SMALL | 6x8 | 6 | ~570 bytes | Default, equivalent to old LCD |
| MEDIUM | 8x16 | 16 | ~1,520 bytes | Readable labels |
| LARGE | 12x24 | 36 | ~3,420 bytes | Speed, prominent values |

**Total font storage**: ~5.5 KB (fits in 128 KB flash easily)

### 7.2 Font Struct

```c
typedef struct {
    const uint8_t *data;       // Bitmap data (column-major, 1 bit/pixel)
    uint8_t width;             // Character width in pixels
    uint8_t height;            // Character height in pixels
    uint8_t first_char;        // First character code (0x20)
    uint8_t last_char;         // Last character code (0x7E)
    uint8_t bytes_per_char;    // width * ((height + 7) / 8)
} lcd_font_t;

extern const lcd_font_t font_small;   // 6x8
extern const lcd_font_t font_medium;  // 8x16
extern const lcd_font_t font_large;   // 12x24
```

### 7.3 Rendering

`lcd_text(x, y, str, font)` iterates characters, looks up bitmap, and blits into
framebuffer using `lcd_pixel()` calls. Column-major storage means vertical pixel
strips are contiguous — efficient for the UC1611S page organization.

---

## 8. Changes to Existing Code

### 8.1 `ui.c` — Rewrite

Old API (character-based):
```c
lcd_set_cursor(0, 0);
lcd_printfln("%d km/h %d.%dkm", speed, d_km, d_01);
```

New API (pixel-based):
```c
char buf[32];
snprintf(buf, sizeof(buf), "%d km/h %d.%dkm", speed, d_km, d_01);
lcd_text(0, 0, buf, font_small);
lcd_flush();
```

The formatting logic (`snprintf`, `unit_2_line`, etc.) stays mostly the same.
The main change is replacing `lcd_printfln`/`lcd_println` with `snprintf` + `lcd_text`.

### 8.2 `system.c` — Port

- Backlight: change `BACKLIGHT_Pin`/`BACKLIGHT_GPIO_Port` to `LIGHT_Pin`/`LIGHT_GPIO_Port`
- Buttons: change EXTI callbacks from `S1`/`S2`/`S3` to `BTN1`/`BTN2`/`BTN3`
- Remove `lcd_println` from `panic()` — use `lcd_text` + `lcd_flush` instead

### 8.3 `Makefile` — Add Sources

Add to `C_SOURCES`:
```makefile
Src/lcd/lcd_uc1611s.c \
Src/lcd/lcd_uc1611s_graphics.c \
```

Add to `C_INCLUDES`:
```makefile
-IInc/lcd
```

### 8.4 `main.c` — Wire Up

In `USER CODE BEGIN 2`:
```c
lcd_init();
logic_init();
```

In `USER CODE BEGIN WHILE`:
```c
logic_update();
lcd_flush();
```

Add EXTI callbacks for buttons (BTN1/2/3 on PA4/PA5/PA6).

### 8.5 Tests — New Fake

Replace `hd44780_puppet.hpp` with `uc1611s_puppet.hpp`:
- `lcd_get_framebuffer()` — returns pointer to internal fb for inspection
- `lcd_get_pixel(x, y)` — read pixel from fb
- Text extraction helpers (render fb to text lines for comparison)

---

## 9. SPI Transaction Pattern

Every SPI transaction follows this pattern:

```c
// Assert chip select (active low)
HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET);

// Set command/data mode
HAL_GPIO_WritePin(LCD_CD_GPIO_Port, LCD_CD_Pin,
                  is_data ? GPIO_PIN_SET : GPIO_PIN_RESET);

// Send byte(s) via HAL SPI
HAL_SPI_Transmit(&hspi1, &byte, 1, HAL_MAX_DELAY);

// Deassert chip select
HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET);
```

For framebuffer flush, CS is held low for the entire transfer:
```c
HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET);
HAL_GPIO_WritePin(LCD_CD_GPIO_Port, LCD_CD_Pin, GPIO_PIN_SET); // data mode
HAL_SPI_Transmit(&hspi1, fb, sizeof(fb), HAL_MAX_DELAY);
HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET);
```

---

## 10. Open Questions / Future Work

1. **Orientation**: Code should support both via LCD mapping register (0xC0 + flags).
   Default for testing: horizontal. Final orientation TBD on hardware.

2. **Contrast tuning**: Initial value 0x8F from u8g2 reference. May need per-module
   adjustment. Register 0x81 allows runtime tuning.

3. **Font quality**: Initial fonts are simple monospace bitmaps. Can be refined later
   with proportional spacing or anti-aliasing if desired.

4. **RST pin**: Not assigned. Use software reset (0xE2). Add hardware reset later if
   needed for stability.

5. **DMA**: Current plan uses polling SPI (`HAL_SPI_Transmit`). Could add DMA for
   non-blocking framebuffer transfers as an optimization.

6. **Partial updates**: `lcd_flush_region()` for dirty-rectangle tracking — optimization
   for later if full-frame flush is too slow.

---

## 11. Implementation Order

1. **lcd_config.h** — defines, pin mappings, screen dimensions
2. **lcd_uc1611s.c/h** — low-level driver (init, command, flush)
3. **lcd_font_small.h** — first font (6x8)
4. **lcd_uc1611s_graphics.c/h** — framebuffer, pixel ops, text
5. **Smoke test** — init display, fill screen, show text
6. **ui.c rewrite** — migrate from character to pixel API
7. **system.c port** — backlight, buttons
8. **main.c wiring** — integrate everything
9. **Remaining fonts** — medium, large
10. **Tests** — fake driver, unit tests
