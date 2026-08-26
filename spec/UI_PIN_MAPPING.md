# UI Unit Pin Mapping — STM32G431KB (NUCLEO-G431KB)

> Target MCU: STM32G431KB (LQFP32, 32 pins)
> Display: EA DOGXL240W-7 (240×128 COG LCD, SPI, UC1611S controller)
> EEPROM: 24L256 (I²C)
> CAN transceiver: MCP2562 (CAN 2.0)
> Temperature sensor: LM35 (analog)
> Debug: USART1 (synchronous)

---

## 0. Package Constraint — LQFP32 Available Pins

Only **24 GPIOs** are bonded out on the 32-pin package:

| Port | Available Pins | NOT Available |
|------|---------------|---------------|
| PA | PA0–PA15 (16) | — |
| PB | PB0, PB3–PB8 (7) | **PB1, PB2, PB9–PB15 do not exist** |
| PF | PF0, PF1 (oscillator only) | — |
| PG | — | PG10 = NRST (not GPIO) |

**PA13/PA14** are permanently reserved for SWD debug.

---

## 1. Complete Pin Mapping

| Pin | Function | Peripheral / Mode | AF | Dir | Notes |
|-----|----------|-------------------|-----|-----|-------|
| PA0 | LM35 temperature sensor | ADC1_IN1 | Analog | In | 10 mV/°C, single conversion |
| PA1 | *(free)* | — | — | — | Not assigned |
| PA2 | VCP TX (ST-LINK) | USART2_TX | AF7 | Out | SB1 ON (default) |
| PA3 | VCP RX (ST-LINK) | USART2_RX | AF7 | In | SB12 ON (default) |
| PA4 | Button S1 | GPIO_EXTI4 | — | In | Pull-up, both edges. Trip 1 reset |
| PA5 | Button S2 | GPIO_EXTI5 | — | In | Pull-up, both edges. Trip 2 reset |
| PA6 | Button S3 | GPIO_EXTI6 | — | In | Pull-up, both edges. Display mode / backlight |
| PA7 | LCD Backlight | GPIO_Output | — | Out | Active HIGH (on/off) |
| PA8 | USART1 Clock | USART1_CK | AF7 | Out | Synchronous mode clock output |
| PA9 | Debug USART TX | USART1_TX | AF7 | Out | CN3 pin 2 (labeled RX on board) |
| PA10 | Debug USART RX | USART1_RX | AF7 | In | CN3 pin 1 (labeled TX on board) |
| PA11 | CAN RX | FDCAN1_RX | AF9 | In | From MCP2562 RXD |
| PA12 | CAN TX | FDCAN1_TX | AF9 | Out | To MCP2562 TXD |
| PA13 | SWD debug data | SWDIO | AF0 | I/O | **Never reconfigure** |
| PA14 | SWD debug clock | SWCLK | AF0 | In | **Never reconfigure** |
| PA15 | EEPROM SCL | I2C1_SCL | AF4 | Out | SB3 ON (default) — Arduino A5 |
| PB0 | *(free)* | — | — | — | Not assigned |
| PB3 | SPI1 Clock | SPI1_SCK | AF5 | Out | **Cut SB4** to free from SWO trace |
| PB4 | LCD Chip Select | GPIO_Output | — | Out | Active LOW |
| PB5 | SPI1 MOSI | SPI1_MOSI | AF5 | Out | User LED LD2 on this pin |
| PB6 | LCD Command/Data | GPIO_Output | — | Out | HIGH = data, LOW = command |
| PB7 | EEPROM SDA | I2C1_SDA | AF4 | I/O | SB2 ON (default) — Arduino A4 |
| PB8 | *(not configured)* | — | — | — | BOOT0 pin, locked in .ioc |

**Total pins used:** 18 (of 24 available GPIOs + 2 SWD)
**Free GPIOs:** PA1, PB0, PB8-BOOT0 (3 free pins available)

---

## 2. Peripheral Pin Assignments

### 2.1 SPI1 — EA DOGXL240W-7 Display

| SPI Signal | MCU Pin | AF | Display Pin | Notes |
|------------|---------|-----|-------------|-------|
| SCK | PB3 | AF5 | 38 (SCK) | Cut SB4 to free from SWO trace |
| MOSI | PB5 | AF5 | 39 (SDA) | User LED LD2 on same pin — acceptable |
| CS (GPIO) | PB4 | — | 36 (CS1) | Direct GPIO, active LOW |
| CD (GPIO) | PB6 | — | 34 (CD) | HIGH = data, LOW = command |
| Backlight | PA7 | — | — | GPIO toggle (on/off) |

**SPI config:** Mode 0 (CPOL=0, CPHA=0), 8-bit, MSB first. 5.3125 Mbit/s (prescaler /32 from 170 MHz APB2).

### 2.2 FDCAN1 — CAN Bus

| Signal | MCU Pin | AF | Transceiver Pin | Notes |
|--------|---------|-----|-----------------|-------|
| TX | PA12 | AF9 | MCP2562 TXD | |
| RX | PA11 | AF9 | MCP2562 RXD | |

**FDCAN config:** CAN 2.0B compatible, ~447 kbit/s.
Clock source: PLL (170 MHz), FDCAN prescaler /76, TimeSeg1=2, TimeSeg2=2.

### 2.3 I2C1 — 24L256 EEPROM

| Signal | MCU Pin | AF | EEPROM Pin | Notes |
|--------|---------|-----|------------|-------|
| SCL | PA15 | AF4 | 6 (SCL) | SB3 ON (default) — Arduino A5 |
| SDA | PB7 | AF4 | 5 (SDA) | SB2 ON (default) — Arduino A4 |

**I2C config:** Standard mode (100 kHz), 7-bit addressing.

### 2.4 USART1 — Debug / Logging (Synchronous)

| Signal | MCU Pin | AF | Notes |
|--------|---------|-----|-------|
| TX | PA9 | AF7 | CN3 pin 2 (labeled RX — known board quirk) |
| RX | PA10 | AF7 | CN3 pin 1 (labeled TX — known board quirk) |
| CK | PA8 | AF7 | Synchronous clock output |

**USART1 config:** Synchronous mode, 170 MHz bus clock.

### 2.5 USART2 — VCP (ST-LINK)

| Signal | MCU Pin | AF | Notes |
|--------|---------|-----|-------|
| TX | PA2 | AF7 | Connected to ST-LINK VCP via SB1 |
| RX | PA3 | AF7 | Connected to ST-LINK VCP via SB12 |

**USART2 config:** VCP link to ST-LINK. SB1 and SB12 remain ON (default).

### 2.6 ADC1 — LM35 Temperature Sensor

| Signal | MCU Pin | ADC Channel | Notes |
|--------|---------|-------------|-------|
| VOUT | PA0 | ADC1_IN1 | 10 mV/°C, 3.3V reference |

**ADC config:** 12-bit, single conversion, software trigger.

### 2.7 GPIO — Buttons

| Button | MCU Pin | EXTI Line | IRQ | Purpose |
|--------|---------|-----------|-----|---------|
| S1 | PA4 | EXTI4 | EXTI4_IRQn | Long press: reset trip 1 |
| S2 | PA5 | EXTI5 | EXTI9_5_IRQn | Long press: reset trip 2 |
| S3 | PA6 | EXTI6 | EXTI9_5_IRQn | Short: cycle display mode. Long: toggle backlight |

**Config:** Internal pull-up, rising + falling edge interrupt.

### 2.8 GPIO — Outputs

| Function | MCU Pin | Notes |
|----------|---------|-------|
| LCD Backlight | PA7 | Active HIGH, on/off toggle |
| LCD CS | PB4 | Active LOW |
| LCD CD | PB6 | HIGH = data, LOW = command |

---

## 3. Conflict Analysis

### 3.1 Pin-Level AF Conflicts — None

Every pin serves exactly one function. All alternate function selections are mutually exclusive:

| Pin | Assigned Function | Other AFs on Pin | Conflict? |
|-----|-------------------|------------------|-----------|
| PA0 | ADC1_IN1 | TIM2_CH1 (AF1), LPUART1_CTS (AF8), OPAMP1_OUT (AF12) | **No** — only ADC used |
| PA1 | *(free)* | TIM2_CH2 (AF1), TIM15_CH1 (AF2), USART2_RTS (AF7), LPUART1_RTS (AF8) | **No** — pin free |
| PA2 | USART2_TX (AF7) | TIM2_CH3 (AF1), LPUART1_TX (AF8), COMP2_OUT (AF10) | **No** — only USART2 used |
| PA3 | USART2_RX (AF7) | TIM2_CH4 (AF1), LPUART1_RX (AF8), COMP2_OUT (AF10) | **No** — only USART2 used |
| PA4 | GPIO EXTI (BTN1) | SPI1_NSS (AF5), SPI3_NSS (AF6), USART2_CK (AF7), ADC1_IN5, DAC1_OUT1 | **No** — only GPIO used |
| PA5 | GPIO EXTI (BTN2) | ADC1_IN5 (AF5), TIM2_ETR (AF1) | **No** — only GPIO used |
| PA6 | GPIO EXTI (BTN3) | ADC1_IN6 (AF5), TIM3_CH1 (AF1) | **No** — only GPIO used |
| PA7 | GPIO (LIGHT) | ADC1_IN8, SPI1_MOSI (AF5), TIM3_CH2 (AF2), TIM17_CH1 (AF2), SAI1_FS_B (AF13) | **No** — only GPIO used |
| PA8 | USART1_CK (AF7) | MCO (AF0), TIM1_CH1 (AF1), SPI1_MOSI (AF6), FDCAN1_TX (AF10) | **No** — only USART1 used |
| PA9 | USART1_TX (AF7) | TIM1_CH2 (AF1), SPI1_SCK (AF6), I2C3_SCL (AF8), FDCAN1_RX (AF10), LPUART1_TX (AF11) | **No** — only USART1 used |
| PA10 | USART1_RX (AF7) | TIM1_CH3 (AF1), SPI1_MISO (AF6), I2C3_SDA (AF8), LPUART1_RX (AF11) | **No** — only USART1 used |
| PA11 | FDCAN1_RX (AF9) | TIM1_CH4 (AF1), COMP1_OUT (AF3), I2C1_SDA (AF4), USART1_CTS (AF7), USB_DM (AF10) | **No** — only FDCAN used |
| PA12 | FDCAN1_TX (AF9) | COMP1_OUT (AF3), I2C1_SCL (AF4), USART1_RTS_DE (AF7), TIM1_ETR (AF9), USB_DP (AF10) | **No** — only FDCAN used |
| PA15 | I2C1_SCL (AF4) | JTDI (AF0), TIM2_CH1 (AF1), TIM8_CH1 (AF2), SPI1_NSS (AF5), USART2_RX (AF7), TIM1_BKIN (AF9) | **No** — only I2C1 used |
| PB3 | SPI1_SCK (AF5) | JTDO (AF0), SPI3_SCK (AF6) | **No** — only SPI1 used |
| PB4 | GPIO (LCD_CS) | NJTRST (AF0), TIM3_CH1 (AF2), SPI1_MISO (AF5), SPI3_MISO (AF6) | **No** — only GPIO used |
| PB5 | SPI1_MOSI (AF5) | LPTIM1_IN1 (AF1), TIM3_CH2 (AF2), I2C1_SMBA (AF4) | **No** — only SPI1 used |
| PB6 | GPIO (LCD_CD) | LPTIM1_ETR (AF1), TIM4_CH1 (AF2), SAI1_FS_B (AF3), I2C1_SCL (AF4), USART1_TX (AF7) | **No** — only GPIO used |
| PB7 | I2C1_SDA (AF4) | LPTIM1_IN2 (AF1), TIM4_CH2 (AF2), SAI1_FS_B (AF3), USART1_RX (AF7) | **No** — only I2C1 used |

### 3.2 Board-Level Constraints — All Satisfied

| Constraint | Status | Action Required |
|------------|--------|-----------------|
| PA13 = SWDIO | Reserved | None |
| PA14 = SWCLK | Reserved | None |
| PG10/NRST = Reset | Not used as GPIO | None |
| PB3 = SWO (SB4=ON) | Used for SPI1_SCK | **Cut SB4** |
| PB5 = User LED (LD2) | Used for SPI1_MOSI | LED toggles with MOSI — acceptable |
| PB7 = I2C SDA (SB2=ON) | Used for I2C1_SDA | Keep SB2 ON |
| PA15 = I2C SCL (SB3=ON) | Used for I2C1_SCL | Keep SB3 ON |
| PA2 = VCP TX (SB1=ON) | Used for USART2_TX | Keep SB1 ON |
| PA3 = VCP RX (SB12=ON) | Used for USART2_RX | Keep SB12 ON |

### 3.3 Peripheral Resource Conflicts — None

| Resource | Used By | Conflicts With |
|----------|---------|----------------|
| SPI1 | Display | Nothing else |
| I2C1 | EEPROM | Nothing else |
| FDCAN1 | CAN bus | Nothing else |
| USART1 | Debug (synchronous) | Nothing else |
| USART2 | VCP (ST-LINK) | Nothing else |
| ADC1 | LM35 | Nothing else |
| EXTI4 | Button S1 | Nothing else |
| EXTI5 | Button S2 | Nothing else |
| EXTI6 | Button S3 | Nothing else |

---

## 4. NUCLEO Board Modifications Required

| Solder Bridge | Default | Required State | Reason |
|---------------|---------|----------------|--------|
| SB1 (VCP TX) | ON | **ON** | Keep PA2 as USART2_TX for VCP |
| SB12 (VCP RX) | ON | **ON** | Keep PA3 as USART2_RX for VCP |
| SB2 (I2C SDA) | ON | **ON** | Keep PB7 as I2C1_SDA on A4 |
| SB3 (I2C SCL) | ON | **ON** | Keep PA15 as I2C1_SCL on A5 |
| SB4 (SWO) | ON | **CUT** | Free PB3 for SPI1_SCK |
| SB8 (PF0→D7) | OFF | OFF | Not needed |
| SB11 (PF1→D8) | OFF | OFF | Not needed |
| SB13 (MCO→HSE) | OFF | OFF | Not used — FDCAN uses PLL clock |
| SB9+SB10 (HSE XTAL) | OFF | OFF | Not needed |

---

## 5. Old vs New Pin Comparison

| Function | Old MCU (STM32F103C8) | New MCU (STM32G431KB) | Change |
|----------|----------------------|----------------------|--------|
| LCD RS | PA0 | *(removed — SPI display)* | Replaced by CD (PB6) |
| LCD RW | PA1 | *(removed — SPI display)* | Not needed in SPI mode |
| LCD E | PA2 | *(removed — SPI display)* | Not needed in SPI mode |
| LCD D4–D7 | PA3–PA6 | *(removed — SPI display)* | Replaced by SPI |
| LCD SPI SCK | — | PB3 | **New** |
| LCD SPI MOSI | — | PB5 | **New** |
| LCD CS | — | PB4 | **New** |
| LCD CD | — | PB6 | **New** |
| LCD Backlight | PB10 | PA7 | Moved |
| CAN TX | PA12 | PA12 | Same pin! |
| CAN RX | PA11 | PA11 | Same pin! |
| I2C1 SCL | PB6 | PA15 | Moved |
| I2C1 SDA | PB7 | PB7 | Same |
| USART1 TX | PA9 | PA9 | Same |
| USART1 RX | PA10 | PA10 | Same |
| ADC (temp) | PB1 (ADC1_IN9) | PA0 (ADC1_IN1) | Moved |
| Button S1 | PB5 | PA4 | Moved |
| Button S2 | PB4 | PA5 | Moved |
| Button S3 | PB3 | PA6 | Moved |
| Buzzer | PB12 | — | Not assigned in current firmware |
| VCP TX | — | PA2 (USART2) | Uses SB1 (default) |
| VCP RX | — | PA3 (USART2) | Uses SB12 (default) |

---

## 6. Free Pins for Future Expansion

**3 free pins:** PA1, PB0, PB8-BOOT0.

Additional expansion options:
- **Final PCB with STM32G431CB (LQFP48):** adds PB1, PB2, PB9–PB15, PC13–PC15 for 15+ extra GPIOs

---

## 7. Clock Configuration Notes

- **System clock:** 170 MHz (PLL from HSI16, PLLM=/4, PLLN=85)
- **APB1/APB2:** 170 MHz (no prescaler)
- **FDCAN clock:** 170 MHz from PLL; prescaler /76, TimeSeg1=2, TimeSeg2=2 → ~447 kbit/s
- **SPI1 clock:** 170 MHz APB2 / prescaler 32 = 5.3125 Mbit/s
- **I2C1 clock:** 170 MHz (timing register 0x40B285C2 for 100 kHz standard mode)
- **USART1/USART2:** 170 MHz bus clock

---

## 8. Summary

**Key design decisions:**
1. **FDCAN1 on PA11/PA12** — confirmed as CubeMX default for G431; same pins as old bxCAN design
2. **SPI1 on PB3/PB5** — only SPI1 option that doesn't conflict with I2C1 or FDCAN1; MOSI-only (no MISO)
3. **I2C1 on PA15/PB7** — uses default NUCLEO I2C solder bridges (SB2/SB3 ON)
4. **USART1 on PA8/PA9/PA10** — synchronous mode for debug
5. **USART2 on PA2/PA3** — VCP link to ST-LINK (SB1/SB12 ON)
6. **Buttons on PA4/PA5/PA6** — GPIO EXTI with internal pull-up
7. **LCD control signals on PB4 (CS), PB6 (CD), PA7 (backlight)** — GPIO outputs
8. **3 free pins remain** — PA1, PB0, PB8-BOOT0 available for expansion

**Hardware modifications:** Cut SB4 (free PB3 for SPI1_SCK). Keep all other solder bridges at default.

**Errata in MCU_PINS.md:** The AF table lists FDCAN1_TX on PA8 (AF10) and FDCAN1_RX on PA9 (AF10), but CubeMX does not offer FDCAN on these pins for the G431KB. FDCAN1 is confirmed on PA11/PA12 (AF9) per CubeMX and ST Community reports. The MCU_PINS.md FDCAN1 section (listing PA8/PB9/PB10 for TX and PA9/PB11/PB12 for RX) also contradicts the AF table and should be corrected.
