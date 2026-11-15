# STM32G431KB Pin Reference & NUCLEO-G431KB Board Constraints

> Source: DS12589 Rev 6 (datasheet), RM0440 (reference manual), UM2397 Rev 2 (NUCLEO-G431KB user manual)
> Package: UFQFPN32 / LQFP32 — 32 pins total
>
> **⚠️ This document covers the full G4 family. For the UFQFPN32 package (STM32G431KB),
> only PA0–PA15, PB0, PB3–PB8, and PF0–PF1 are bonded out. Pins marked ✕ in the AF
> tables do NOT exist on the 32-pin package. See Section 7, Note 1 for details.**

---

## 1. STM32G431KB LQFP32 Pin Map

| Pin | Name | Type | Notes |
|-----|------|------|-------|
| 1 | VDD | Power | 1.71–3.6V |
| 2 | PF0 / OSC_IN | I/O | HSE oscillator input |
| 3 | PF1 / OSC_OUT | I/O | HSE oscillator output |
| 4 | PG10 / NRST | I/O | Reset (active low) — **not available as GPIO** |
| 5 | PA0 | I/O | ADC1_IN1 |
| 6 | PA1 | I/O | ADC1_IN2 |
| 7 | PA2 | I/O | ADC1_IN3 |
| 8 | PA3 | I/O | ADC1_IN4 |
| 9 | PA4 | I/O | ADC1_IN5 |
| 10 | PA5 | I/O | ADC1_IN6 |
| 11 | PA6 | I/O | ADC1_IN7 |
| 12 | PA7 | I/O | ADC1_IN8 |
| 13 | PA8 | I/O | — |
| 14 | PA9 | I/O | — |
| 15 | PA10 | I/O | — |
| 16 | PA11 | I/O | ADC2_IN11 |
| 17 | PA12 | I/O | ADC2_IN12 |
| 18 | VSS | Power | Ground |
| 19 | PA13 | I/O | **SWDIO** (debug) |
| 20 | PA14 | I/O | **SWCLK** (debug) |
| 21 | VDDA | Power | Analog supply |
| 22 | VSS | Power | Ground |
| 23 | PB0 | I/O | ADC1_IN1 / ADC2_IN1 |
| 24 | VDD | Power | — |
| 25 | PB8 / BOOT0 | I/O | BOOT0 selection |
| 26 | PB7 | I/O | — |
| 27 | PB6 | I/O | — |
| 28 | PB5 | I/O | — |
| 29 | PB4 | I/O | — |
| 30 | PB3 | I/O | — |
| 31 | PA15 | I/O | — |
| 32 | VSS | Power | Ground |

---

## 2. Alternate Function Mapping (DS12589 Table 13)

### AF Number Assignment Summary

| AF | Peripheral Group |
|----|-----------------|
| AF0 | SYS_AF: MCO, SWD (SWDIO/SWCLK), JTAG (JTDI/JTDO/NJTRST), TRACE, RTC_REFIN |
| AF1 | LPTIM1, TIM1/TIM2/TIM15/TIM16/TIM17 |
| AF2 | TIM1/TIM2/TIM3/TIM4/TIM8/TIM15, GPCOMP |
| AF3 | SAI1, USB, TIM8/TIM15, GPCOMP, I2C3 |
| AF4 | I2C1/I2C2/I2C3, TIM1/TIM8/TIM16/TIM17 |
| AF5 | SPI1/SPI2/SPI3, I2S2/I2S3, UART4 |
| AF6 | SPI2/SPI3, I2S2/I2S3, TIM1/TIM8 |
| AF7 | USART1/USART2/USART3 |
| AF8 | I2C3/I2C4, UART4, LPUART1, GPCOMP |
| AF9 | TIM1/TIM8/TIM15, FDCAN1 |
| AF10 | TIM2/TIM3/TIM4/TIM8/TIM17, LPTIM1, FDCAN1 |
| AF11 | LPUART1, SAI1, TIM1 |
| AF12 | SAI1, OPAMP2 |
| AF13 | UART4, SAI1, TIM2/TIM15, UCPD1 |
| AF14 | Reserved |
| AF15 | EVENTOUT |

---

### PORT A — Full AF Table

| Pin | AF0 | AF1 | AF2 | AF3 | AF4 | AF5 | AF6 | AF7 | AF8 | AF9 | AF10 | AF11 | AF12 | AF13 | AF15 |
|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|------|------|------|------|------|
| PA0 | — | TIM2_CH1 | — | COMP1_OUT | — | — | — | USART2_CTS | — | — | COMP1_OUT | LPUART1_CTS | OPAMP1_OUT | — | EVENTOUT |
| PA1 | — | TIM2_CH2 | TIM15_CH1 | COMP1_OUT | — | — | — | USART2_RTS_DE | — | — | COMP1_OUT | LPUART1_RTS_DE | OPAMP2_OUT | SAI1_MCLK_A | EVENTOUT |
| PA2 | — | TIM2_CH3 | TIM15_CH2 | — | — | — | — | USART2_TX | LPUART1_TX | — | COMP2_OUT | — | — | SAI1_SCK_A | EVENTOUT |
| PA3 | — | TIM2_CH4 | — | COMP1_OUT | — | — | — | USART2_RX | LPUART1_RX | — | COMP2_OUT | — | — | SAI1_FS_A | EVENTOUT |
| PA4 | — | — | — | COMP1_OUT | — | SPI1_NSS | SPI3_NSS | USART2_CK | — | — | — | — | — | — | EVENTOUT |
| PA5 | — | TIM2_CH1 | TIM2_ETR | COMP1_OUT | — | SPI1_SCK | — | — | — | — | — | — | — | — | EVENTOUT |
| PA6 | — | TIM3_CH1 | TIM15_CH1 | — | TIM16_BKIN | SPI1_MISO | — | — | — | COMP2_OUT | — | — | — | SAI1_SCK_B | EVENTOUT |
| PA7 | — | TIM3_CH2 | TIM17_CH1 | COMP1_OUT | TIM17_CH2 | SPI1_MOSI | — | — | — | — | — | — | — | SAI1_FS_B | EVENTOUT |
| PA8 | MCO | TIM1_CH1 | — | — | — | — | SPI1_MOSI | USART1_CK | — | TIM8_BKIN2 | FDCAN1_TX | — | — | — | EVENTOUT |
| PA9 | — | TIM1_CH2 | — | — | — | — | SPI1_SCK | USART1_TX | I2C3_SCL | — | FDCAN1_RX | LPUART1_TX | — | — | EVENTOUT |
| PA10 | — | TIM1_CH3 | — | — | — | — | SPI1_MISO | USART1_RX | I2C3_SDA | — | — | LPUART1_RX | — | — | EVENTOUT |
| PA11 | — | TIM1_CH4 | — | COMP1_OUT | I2C1_SDA | — | — | USART1_CTS | — | FDCAN1_RX¹ | USB_DM | — | — | — | EVENTOUT |
| PA12 | — | — | — | COMP1_OUT | I2C1_SCL | — | — | USART1_RTS_DE | — | FDCAN1_TX¹ | USB_DP | — | — | — | EVENTOUT |
| PA13 | SWDIO | — | — | — | I2C1_SCL | — | — | — | — | — | — | — | — | — | EVENTOUT |
| PA14 | SWCLK | — | — | — | I2C1_SDA | — | — | — | — | TIM1_BKIN | USART2_TX | SAI1_FS_B | — | — | EVENTOUT |
| PA15 | JTDI | TIM2_CH1 | TIM8_CH1 | — | I2C1_SCL | SPI1_NSS | SPI3_NSS | USART2_RX | UART4_RTS_DE | TIM1_BKIN | TIM2_ETR | — | — | — | EVENTOUT |

> ¹ AF9 on PA11/PA12: The datasheet (DS12589 Table 13) lists TIM1_BKIN2 (PA11) and
> TIM1_ETR (PA12) on AF9. However, **CubeMX and ST community reports confirm FDCAN1_RX
> on PA11 and FDCAN1_TX on PA12 as AF9** for the G431KB. The entries above reflect the
> CubeMX-verified mapping. If using a different G4 variant, verify against your specific
> datasheet revision.

### PORT B — Full AF Table

> **Pins marked ✕ are NOT bonded out on UFQFPN32/LQFP32 (STM32G431KB).** They exist on
> LQFP48+ packages (G431CB, G431RB, etc.) but cannot be used on the 32-pin variant.

| Pin | AF0 | AF1 | AF2 | AF3 | AF4 | AF5 | AF6 | AF7 | AF8 | AF9 | AF10 | AF11 | AF12 | AF13 | AF15 |
|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|------|------|------|------|------|
| PB0 | — | — | TIM3_CH3 | TIM1_ETR | TIM16_CH1 | — | SPI1_MOSI | — | — | TIM3_ETR | — | — | — | OPAMP1_OUT | EVENTOUT |
| PB1 ✕ | — | — | TIM3_CH4 | — | TIM16_CH1N | — | — | USART3_RTS_DE | — | — | — | LPUART1_RTS_DE | OPAMP1_OUT | OPAMP3_OUT | EVENTOUT |
| PB2 ✕ | — | — | — | — | — | — | — | — | — | — | — | — | — | — | EVENTOUT |
| PB3 | JTDO | — | — | — | — | SPI1_SCK | SPI3_SCK | — | — | — | — | — | — | — | EVENTOUT |
| PB4 | NJTRST | — | TIM3_CH1 | — | — | SPI1_MISO | SPI3_MISO | — | — | — | — | — | — | — | EVENTOUT |
| PB5 | — | LPTIM1_IN1 | TIM3_CH2 | — | I2C1_SMBA | SPI1_MOSI | — | — | — | — | — | — | — | — | EVENTOUT |
| PB6 | — | LPTIM1_ETR | TIM4_CH1 | SAI1_FS_B | I2C1_SCL | — | — | USART1_TX | — | — | — | — | — | — | EVENTOUT |
| PB7 | — | LPTIM1_IN2 | TIM4_CH2 | — | I2C1_SDA | — | — | USART1_RX | — | — | — | — | — | — | EVENTOUT |
| PB8 | — | — | TIM4_CH3 | SAI1_SCK_A | — | SPI1_SCK | — | — | I2C1_SCL | TIM16_CH1 | — | — | — | SAI1_FS_B | EVENTOUT |
| PB9 ✕ | — | TIM17_CH1 | TIM4_CH4 | SAI1_D2 | I2C1_SDA | IR_OUT | — | USART3_TX | COMP2_OUT | FDCAN1_TX | TIM8_CH3 | TIM1_CH3N | SAI1_FS_A | — | EVENTOUT |
| PB10 ✕ | — | — | TIM2_CH3 | SAI1_SCK_B | I2C2_SCL | SPI2_SCK | — | USART3_TX | — | FDCAN1_TX | — | — | — | — | EVENTOUT |
| PB11 ✕ | — | — | TIM2_CH4 | SAI1_D2 | I2C2_SDA | — | — | USART3_RX | — | FDCAN1_RX | — | — | — | — | EVENTOUT |
| PB12 ✕ | — | LPTIM1_OUT | — | COMP2_OUT | I2C2_SMBA | SPI2_NSS | — | USART3_CK | — | FDCAN1_RX | — | — | — | — | EVENTOUT |
| PB13 ✕ | — | — | — | COMP4_OUT | I2C2_SCL | SPI2_SCK | TIM1_CH1N | USART3_CTS | LPUART1_CTS | — | — | — | — | — | EVENTOUT |
| PB14 ✕ | — | TIM15_CH1 | — | — | I2C2_SDA | SPI2_MISO | TIM1_CH2N | USART3_RTS_DE | COMP4_OUT | — | — | — | — | — | EVENTOUT |
| PB15 ✕ | RTC_REFIN | TIM15_CH2 | TIM15_CH1N | COMP3_OUT | TIM1_CH3N | SPI2_MOSI | — | — | — | — | — | — | — | — | EVENTOUT |

### PORT C — Full AF Table

> **All PORT C pins are ✕ NOT available on UFQFPN32/LQFP32 (STM32G431KB).**
> Included for reference when using LQFP48+ packages.

| Pin | AF0 | AF1 | AF2 | AF3 | AF4 | AF5 | AF6 | AF7 | AF8 | AF15 |
|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|------|
| PC0 ✕ | — | — | — | — | — | — | — | — | LPTIM1_IN1 | EVENTOUT |
| PC1 ✕ | — | — | — | — | — | — | — | — | LPTIM1_OUT | EVENTOUT |
| PC2 ✕ | — | — | — | — | — | — | — | — | LPTIM1_IN3 | EVENTOUT |
| PC3 ✕ | — | — | — | — | — | — | — | — | LPTIM1_ETR | EVENTOUT |
| PC4 ✕ | — | — | — | COMP1_OUT | — | — | — | — | LPTIM1_IN2 | EVENTOUT |
| PC5 ✕ | — | — | — | COMP1_OUT | — | — | — | — | LPTIM1_IN1 | EVENTOUT |
| PC6 ✕ | — | TIM3_CH1 | TIM8_CH1 | — | — | — | I2S2_MCK | — | — | EVENTOUT |
| PC7 ✕ | — | TIM3_CH2 | TIM8_CH2 | — | — | — | — | — | — | EVENTOUT |
| PC8 ✕ | — | TIM3_CH3 | TIM8_CH3 | — | — | — | — | — | — | EVENTOUT |
| PC9 ✕ | — | TIM3_CH4 | TIM8_CH4 | — | — | — | — | — | — | EVENTOUT |
| PC10 ✕ | — | — | TIM8_CH1N | — | — | UART4_TX | SPI3_SCK | USART3_TX | — | EVENTOUT |
| PC11 ✕ | — | — | TIM8_CH2N | — | — | UART4_RX | SPI3_MISO | USART3_RX | I2C3_SDA | EVENTOUT |
| PC12 ✕ | — | — | TIM8_CH3N | — | TIM8_CH4 | — | SPI3_MOSI | USART3_CK | UCPD1_FRSTX | EVENTOUT |
| PC13 ✕ | — | — | TIM1_BKIN | — | TIM1_CH1N | — | TIM8_CH4N | — | — | EVENTOUT |
| PC14 ✕ | — | — | — | — | — | — | — | — | — | EVENTOUT |
| PC15 ✕ | — | — | — | — | — | — | — | — | — | EVENTOUT |

### PORT F — Full AF Table

> **PF2, PF9, PF10 are ✕ NOT available on UFQFPN32/LQFP32.**
> PF0/PF1 are available but primarily for HSE oscillator (requires SB8/SB11 on NUCLEO).

| Pin | AF0 | AF1 | AF2 | AF3 | AF4 | AF5 | AF6 | AF7 | AF8 | AF15 |
|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|------|
| PF0 | — | — | — | — | I2C2_SDA | SPI2_NSS | TIM1_CH3N | — | — | EVENTOUT |
| PF1 | — | — | — | — | — | SPI2_SCK | — | — | — | EVENTOUT |
| PF2 ✕ | — | — | — | — | I2C2_SMBA | — | — | — | — | EVENTOUT |
| PF9 ✕ | — | — | TIM15_CH1 | — | — | SPI2_SCK | — | — | — | EVENTOUT |
| PF10 ✕ | — | — | TIM15_CH2 | — | — | SPI2_SCK | — | — | — | EVENTOUT |

### PORT G

| Pin | AF0 | AF15 | Notes |
|-----|-----|------|-------|
| PG10 | MCO | EVENTOUT | **NRST pin — not available as GPIO** on any package |

---

## 3. Peripheral-to-Pin Conflict Matrix

When a peripheral is mapped to a pin, **all other AFs on that pin become unavailable**. Below are the key conflicts for each peripheral.

> **Pins marked ✕ are NOT available on UFQFPN32.** Conflict info is included for LQFP48+ reference.

### I2C1

| Signal | Available Pins (UFQFPN32) | AF | Conflicts |
|--------|---------------------------|-----|-----------|
| I2C1_SCL | PA13, PA15, PB6, PB8 | AF4 | PA13=SWDIO, PA15=SPI1_NSS/USART2_RX/JTDI, PB6=USART1_TX, PB8=SPI1_SCK |
| I2C1_SDA | PA11, PA14, PB7, PB5 | AF4 | PA11=USB_DM/FDCAN1_RX, PA14=SWCLK/USART2_TX, PB7=USART1_RX, PB5=SPI1_MOSI |
| I2C1_SMBA | PB5 | AF4 | PB5=SPI1_MOSI/TIM3_CH2 |

**I2C1 conflicts (UFQFPN32):**
- PA13+PA14: SWD pins — **cannot use for I2C1** without losing debug
- PA15+PB7: Common NUCLEO I2C pair (SB2/SB3) — conflicts with USART2_RX and USART1_RX
- PB6+PB7: TIM4_CH1/TIM4_CH2 and USART1_TX/USART1_RX — using I2C1 here blocks USART1

### I2C2

| Signal | Available Pins (UFQFPN32) | AF | Conflicts |
|--------|---------------------------|-----|-----------|
| I2C2_SCL | PF0 (oscillator only) | AF4 | PF0=OSC_IN — only available without external crystal |
| I2C2_SDA | PF0 (oscillator only) | AF4 | PF0=OSC_IN — only available without external crystal |
| I2C2_SMBA | PF2 ✕ | AF4 | Not available on UFQFPN32 |

**I2C2 conflicts:**
- **No practical I2C2 option on UFQFPN32** — PB10/PB11/PB12/PB13 are not bonded out
- PF0 can serve as I2C2_SDA but conflicts with HSE oscillator
- PF0: Only available when not using external crystal

### I2C3

| Signal | Available Pins (UFQFPN32) | AF | Conflicts |
|--------|---------------------------|-----|-----------|
| I2C3_SCL | PA9, PA13, PA15, PB8 | AF4 | PA9=USART1_TX/SPI1_SCK, PA13=SWDIO, PA15=JTDI/USART2_RX, PB8=SPI1_SCK |
| I2C3_SDA | PA10, PA14 | AF4 | PA10=USART1_RX, PA14=SWCLK/USART2_TX |

### USART1

| Signal | Available Pins (UFQFPN32) | AF | Conflicts |
|--------|---------------------------|-----|-----------|
| USART1_TX | PA9, PB6 | AF7 | PA9=SPI1_SCK/I2C3_SCL, PB6=TIM4_CH1/I2C1_SCL |
| USART1_RX | PA10, PB7 | AF7 | PA10=SPI1_MISO/I2C3_SDA, PB7=TIM4_CH2/I2C1_SDA |
| USART1_CTS | PA11 | AF7 | PA11=FDCAN1_RX/USB_DM |
| USART1_RTS_DE | PA12 | AF7 | PA12=FDCAN1_TX/USB_DP |
| USART1_CK | PA8 | AF7 | PA8=MCO/TIM1_CH1 |

### USART2

| Signal | Available Pins (UFQFPN32) | AF | Conflicts |
|--------|---------------------------|-----|-----------|
| USART2_TX | PA2, PA14 | AF7 | PA2=LPUART1_TX/COMP2_OUT (default VCP on NUCLEO), PA14=SWCLK |
| USART2_RX | PA3, PA15 | AF7 | PA3=LPUART1_RX/COMP2_OUT (default VCP on NUCLEO), PA15=JTDI/SPI1_NSS |
| USART2_CTS | PA0 | AF7 | PA0=TIM2_CH1/COMP1_OUT |
| USART2_RTS_DE | PA1 | AF7 | PA1=TIM2_CH2/LPUART1_RTS_DE |
| USART2_CK | PA4 | AF7 | PA4=SPI1_NSS/SPI3_NSS |

### USART3

| Signal | Available Pins (UFQFPN32) | AF | Conflicts |
|--------|---------------------------|-----|-----------|
| USART3_TX | **None** ✕ | AF7 | PB9/PB10/PC10 not available on UFQFPN32 |
| USART3_RX | **None** ✕ | AF7 | PB11/PC11 not available on UFQFPN32 |
| USART3_CK | **None** ✕ | AF7 | PB12/PC12 not available on UFQFPN32 |
| USART3_CTS | **None** ✕ | AF7 | PB13 not available on UFQFPN32 |
| USART3_RTS_DE | **None** ✕ | AF7 | PB1/PB14 not available on UFQFPN32 |

> **USART3 is not usable on UFQFPN32** — all USART3 pins require PB9–PB15 or PC pins.

### LPUART1

| Signal | Available Pins (UFQFPN32) | AF | Conflicts |
|--------|---------------------------|-----|-----------|
| LPUART1_TX | PA2, PA9 | AF8 | PA2=USART2_TX (default VCP), PA9=USART1_TX/SPI1_SCK |
| LPUART1_RX | PA3, PA10 | AF8 | PA3=USART2_RX (default VCP), PA10=USART1_RX/SPI1_MISO |
| LPUART1_CTS | PA0 | AF8 | PA0=TIM2_CH1 |
| LPUART1_RTS_DE | PA1 | AF8 | PA1=TIM2_CH2 |

### UART4

| Signal | Available Pins (UFQFPN32) | AF | Conflicts |
|--------|---------------------------|-----|-----------|
| UART4_TX | **None** ✕ | AF5 | PC10 not available on UFQFPN32 |
| UART4_RX | **None** ✕ | AF5 | PC11 not available on UFQFPN32 |
| UART4_RTS_DE | PA15 | AF8 | PA15=JTDI/SPI1_NSS/USART2_RX |

> **UART4 TX/RX not usable on UFQFPN32** — requires PC10/PC11.

### SPI1

| Signal | Available Pins (UFQFPN32) | AF | Conflicts |
|--------|---------------------------|-----|-----------|
| SPI1_SCK | PA5, PA9, PB3 | AF5 | PA5=TIM2_CH1 (SB2 constraint), PA9=USART1_TX/I2C3_SCL, PB3=JTDO/SPI3_SCK |
| SPI1_MISO | PA6, PA10, PB4 | AF5 | PA6=TIM3_CH1 (SB3 constraint), PA10=USART1_RX/I2C3_SDA, PB4=NJTRST/SPI3_MISO |
| SPI1_MOSI | PA7, PA8, PB5 | AF5 | PA7=TIM3_CH2, PA8=USART1_CK/TIM1_CH1, PB5=TIM3_CH2 |
| SPI1_NSS | PA4, PA15 | AF5 | PA4=SPI3_NSS/USART2_CK, PA15=JTDI/USART2_RX |

### SPI2

| Signal | Available Pins (UFQFPN32) | AF | Conflicts |
|--------|---------------------------|-----|-----------|
| SPI2_SCK | PF0, PF1 (oscillator only) | AF5 | PF0=OSC_IN, PF1=OSC_OUT — only available without external crystal |
| SPI2_MISO | **None** ✕ | AF5 | PB14 not available on UFQFPN32 |
| SPI2_MOSI | **None** ✕ | AF5 | PB15/PB1 not available on UFQFPN32 |
| SPI2_NSS | PF0 (oscillator only) | AF5 | PF0=OSC_IN |

> **SPI2 is not practical on UFQFPN32** — MISO/MOSI require PB14/PB15/PB1.

### SPI3

| Signal | Available Pins (UFQFPN32) | AF | Conflicts |
|--------|---------------------------|-----|-----------|
| SPI3_SCK | PB3 | AF6 | PB3=JTDO/SPI1_SCK |
| SPI3_MISO | PB4 | AF6 | PB4=NJTRST/SPI1_MISO |
| SPI3_MOSI | **None** ✕ | AF6 | PC12 not available on UFQFPN32 |
| SPI3_NSS | PA4, PA15 | AF6 | PA4=SPI1_NSS/USART2_CK, PA15=JTDI/USART2_RX |

> **SPI3 MOSI not available on UFQFPN32** — requires PC12.

### FDCAN1

> **⚠️ AF table discrepancy:** The PORT A AF table (Section 2) lists FDCAN1_TX on PA8 (AF10)
> and FDCAN1_RX on PA9 (AF10). However, **CubeMX for the G431KB only offers FDCAN1 on
> PA11 (RX) / PA12 (TX) as AF9**. The PA8/PA9 entries appear to be for a different package
> variant or a datasheet error. PA11/PA12 FDCAN1 is confirmed working by ST Community reports.
> The AF table for PA11/PA12 below reflects the CubeMX-verified mapping.

| Signal | Available Pins (UFQFPN32) | AF | Conflicts |
|--------|---------------------------|-----|-----------|
| FDCAN1_TX | PA12 | AF9 | PA12=USB_DP/USART1_RTS_DE/TIM1_ETR/I2C1_SCL/COMP1_OUT |
| FDCAN1_RX | PA11 | AF9 | PA11=USB_DM/USART1_CTS/TIM1_CH4/I2C1_SDA/COMP1_OUT |

**Remap alternatives (not verified on UFQFPN32 — listed for reference only):**
- PA8 (AF10) = FDCAN1_TX — conflicts with MCO/TIM1_CH1/USART1_CK
- PA9 (AF10) = FDCAN1_RX — conflicts with USART1_TX/SPI1_SCK/I2C3_SCL
- PB8 (AF10) = FDCAN1_TX — remap option (PB9/PB10 not available on UFQFPN32)

### TIM1

| Signal | Available Pins (UFQFPN32) | AF | Conflicts |
|--------|---------------------------|-----|-----------|
| TIM1_CH1 | PA8 | AF1 | PA8=MCO/USART1_CK |
| TIM1_CH2 | PA9 | AF1 | PA9=USART1_TX/SPI1_SCK |
| TIM1_CH3 | PA10 | AF1 | PA10=USART1_RX/SPI1_MISO |
| TIM1_CH4 | PA11 | AF1 | PA11=FDCAN1_RX/USB_DM |
| TIM1_CH1N | PB13 ✕ | AF6 | Not available on UFQFPN32 |
| TIM1_CH2N | PB14 ✕ | AF6 | Not available on UFQFPN32 |
| TIM1_CH3N | PB15 ✕ | AF4 | Not available on UFQFPN32 |
| TIM1_ETR | PA12 | AF9 | PA12=FDCAN1_TX/USB_DP |
| TIM1_BKIN | PA15 | AF9 | PA15=JTDI/USART2_RX |
| TIM1_BKIN2 | PA8 | AF9 | PA8=MCO/USART1_CK |

### TIM2

| Signal | Available Pins (UFQFPN32) | AF | Conflicts |
|--------|---------------------------|-----|-----------|
| TIM2_CH1 | PA0, PA5, PA15 | AF1 | PA0=COMP1_OUT/USART2_CTS, PA5=SPI1_SCK (SB2 constraint), PA15=JTDI/USART2_RX |
| TIM2_CH2 | PA1, PB3 | AF1 | PA1=COMP1_OUT/LPUART1_RTS_DE, PB3=JTDO/SPI1_SCK |
| TIM2_CH3 | PA2 | AF1 | PA2=USART2_TX/LPUART1_TX |
| TIM2_CH4 | PA3 | AF1 | PA3=USART2_RX/LPUART1_RX |
| TIM2_ETR | PA5, PA15 | AF2 | PA5=SPI1_SCK, PA15=JTDI/USART2_RX |

### TIM3

| Signal | Available Pins (UFQFPN32) | AF | Conflicts |
|--------|---------------------------|-----|-----------|
| TIM3_CH1 | PA6, PB4 | AF2 | PA6=SPI1_MISO (SB3 constraint), PB4=NJTRST/SPI1_MISO |
| TIM3_CH2 | PA7, PB5 | AF2 | PA7=SPI1_MOSI, PB5=SPI1_MOSI |
| TIM3_CH3 | PB0 | AF2 | PB0=SPI1_MOSI |
| TIM3_CH4 | PB1 ✕ | AF2 | Not available on UFQFPN32 |
| TIM3_ETR | PB0 | AF9 | PB0=SPI1_MOSI |

### TIM4

| Signal | Available Pins (UFQFPN32) | AF | Conflicts |
|--------|---------------------------|-----|-----------|
| TIM4_CH1 | PB6 | AF2 | PB6=I2C1_SCL/USART1_TX |
| TIM4_CH2 | PB7 | AF2 | PB7=I2C1_SDA/USART1_RX |
| TIM4_CH3 | PB8 | AF2 | PB8=SPI1_SCK/I2C1_SCL |
| TIM4_CH4 | PB9 ✕ | AF2 | Not available on UFQFPN32 |

### TIM8

> **TIM8 is not usable on UFQFPN32** — all TIM8 pins require PC6–PC13.

| Signal | Available Pins (UFQFPN32) | AF | Conflicts |
|--------|---------------------------|-----|-----------|
| TIM8_CH1 | **None** ✕ | AF2 | PC6 not available on UFQFPN32 |
| TIM8_CH2 | **None** ✕ | AF2 | PC7 not available on UFQFPN32 |
| TIM8_CH3 | **None** ✕ | AF2 | PC8 not available on UFQFPN32 |
| TIM8_CH4 | **None** ✕ | AF2 | PC9/PC12 not available on UFQFPN32 |
| TIM8_CH1N | **None** ✕ | AF2 | PC10 not available on UFQFPN32 |
| TIM8_CH2N | **None** ✕ | AF2 | PC11 not available on UFQFPN32 |
| TIM8_CH3N | **None** ✕ | AF2 | PC12 not available on UFQFPN32 |
| TIM8_CH4N | **None** ✕ | AF6 | PC13 not available on UFQFPN32 |
| TIM8_BKIN2 | PA8 | AF9 | PA8=MCO/TIM1_CH1 |

### TIM15

| Signal | Available Pins (UFQFPN32) | AF | Conflicts |
|--------|---------------------------|-----|-----------|
| TIM15_CH1 | PA1, PA6 | AF2 | PA1=TIM2_CH2, PA6=TIM3_CH1 |
| TIM15_CH2 | PA2 | AF2 | PA2=USART2_TX |

### TIM16

| Signal | Available Pins (UFQFPN32) | AF | Conflicts |
|--------|---------------------------|-----|-----------|
| TIM16_CH1 | PA6, PB8 | AF4 | PA6=TIM3_CH1/SPI1_MISO, PB8=SPI1_SCK/I2C1_SCL |
| TIM16_CH1N | PB1 ✕ | AF4 | Not available on UFQFPN32 |

### TIM17

| Signal | Available Pins (UFQFPN32) | AF | Conflicts |
|--------|---------------------------|-----|-----------|
| TIM17_CH1 | PA7 | AF2 | PA7=SPI1_MOSI/TIM3_CH2 |

### USB

| Signal | Available Pins | AF | Conflicts |
|--------|---------------|-----|-----------|
| USB_DM | PA11 | AF10 | PA11=TIM1_CH4/USART1_CTS/I2C1_SDA |
| USB_DP | PA12 | AF10 | PA12=TIM1_ETR/USART1_RTS_DE/I2C1_SCL |

### DAC

| Signal | Available Pins | Notes |
|--------|---------------|-------|
| DAC1_OUT1 | PA4 | Also SPI1_NSS/USART2_CK — use as analog only |
| DAC1_OUT2 | PA5 | Also SPI1_SCK — use as analog only |
| DAC1_OUT3 | PA6 | Also TIM3_CH1/SPI1_MISO — use as analog only |

---

## 4. NUCLEO-G431KB (MB1430) Board Constraints

### 4.1 Permanently Reserved Pins

| Pin | Reserved By | Reason |
|-----|-------------|--------|
| PA13 | ST-LINK SWDIO | SWD debug data line |
| PA14 | ST-LINK SWCLK | SWD debug clock |
| PG10/NRST | ST-LINK NRST | Reset line (pin 4) |

**These 3 pins MUST NOT be reconfigured** if you need debugging/programming.

### 4.2 Solder Bridge Configuration

| SB# | Signal | Default | Effect when ON | Effect when OFF |
|-----|--------|---------|---------------|-----------------|
| SB1 | T_VCP_TX | **ON** | PA2 connected to ST-LINK Virtual COM Port TX | PA2 free (VCP disabled) |
| SB12 | T_VCP_RX | **ON** | PA3 connected to ST-LINK Virtual COM Port RX | PA3 free (VCP disabled) |
| SB2 | ARD_A4 | **ON** | PB7 routed to Arduino A4 (I2C SDA position); **PA5 must be floating input** | PB7 available as D4; PA5 can be used normally |
| SB3 | ARD_A5 | **ON** | PA15 routed to Arduino A5 (I2C SCL position); **PA6 must be floating input** | PA15 available as D5; PA6 can be used normally |
| SB4 | T_SWO | **ON** | PB3 connected to ST-LINK SWO trace | PB3 free (trace disabled) |
| SB8 | PF0 to D7 | **OFF** | PF0-OSC_IN connected to Arduino D7 (CN4 pin 10) | PF0 not routed to header |
| SB11 | PF1 to D8 | **OFF** | PF1-OSC_OUT connected to Arduino D8 (CN4 pin 11) | PF1 not routed to header |
| SB9+SB10 | HSE XTAL | **OFF** | External 24 MHz crystal connected to HSE | No external crystal |
| SB13 | HSE from MCO | **OFF** | ST-LINK MCO (25 MHz) feeds HSE via PF0 | No MCO clock to HSE |
| SB14 | ARD_A2 | **OFF** | PA3 connected to Arduino A2 analog header | PA3 not routed to A2 |

### 4.3 SB2/SB3 Side Effects (Critical!)

When **SB2 is ON** (default):
- PB7 = I2C1_SDA on Arduino A4 position
- **PA5 must be left as floating input** — do NOT configure it as output, alternate function, or analog

When **SB3 is ON** (default):
- PA15 = I2C1_SCL on Arduino A5 position
- **PA6 must be left as floating input** — do NOT configure it as output, alternate function, or analog

### 4.4 Arduino Nano V3 Connector Mapping

#### CN3 (Left Connector)

| CN3 Pin | Arduino Name | STM32 Pin | AF7 Function | Notes |
|---------|-------------|-----------|-------------|-------|
| 1 | TX | PA10 | USART1_RX | Label is misleading — PA10 is USART1 RX |
| 2 | RX | PA9 | USART1_TX | Label is misleading — PA9 is USART1 TX |
| 3 | RST | PG10/NRST | Reset | Connected to NRST |
| 4 | GND | — | Ground | |
| 5 | D2 | PB3 | SPI1_SCK | SB4 default ON (SWO trace) |
| 6 | D3 | PB4 | SPI1_MISO / TIM3_CH1 | |
| 7 | A5 | PA15 | I2C1_SCL | SB3 default ON — I2C on A5 |
| 8 | A4 | PB7 | I2C1_SDA | SB2 default ON — I2C on A4 |
| 9 | A3 | PA4 | ADC1_IN5 | |
| 10 | A2 | PA3 | ADC1_IN4 / USART2_RX | SB14 default OFF |
| 11 | A1 | PA1 | ADC1_IN2 / TIM2_CH2 | |
| 12 | A0 | PA0 | ADC1_IN1 / TIM2_CH1 | |
| 13 | VREF+ | VREF+ | ADC reference | |
| 14 | 3V3 | — | 3.3V output | |
| 15 | GND | — | Ground | |

#### CN4 (Right Connector)

| CN4 Pin | Arduino Name | STM32 Pin | AF Function | Notes |
|---------|-------------|-----------|-------------|-------|
| 1 | D13 | PB5 | SPI1_MOSI | User LED (LD2) connected here |
| 2 | D12 | PB4 | SPI1_MISO / TIM3_CH1 | Shared with CN3 D3 |
| 3 | D11 | PB5 | SPI1_MOSI | Duplicate of D13 |
| 4 | D10 | PA11 | USB_DM / TIM1_CH4 | |
| 5 | D9 | PA8 | TIM1_CH1 / FDCAN1_TX | |
| 6 | D8 | PF1 | OSC_OUT | SB11 default OFF |
| 7 | D7 | PF0 | OSC_IN | SB8 default OFF |
| 8 | D6 | PB6 | I2C1_SCL / USART1_TX | |
| 9 | D5 | PA15 | I2C1_SCL / SPI1_NSS | SB3 default ON — conflicts with A5 |
| 10 | D4 | PB7 | I2C1_SDA / TIM4_CH2 | SB2 default ON — conflicts with A4 |
| 11 | D3 | PB3 | SPI1_SCK | SB4 default ON (SWO) |
| 12 | D2 | PB2 ✕ | GPIO only | **Not available on UFQFPN32** — no bonded pin |
| 13 | GND | — | Ground | |
| 14 | 5V | — | 5V output/input | |
| 15 | VIN | — | External power | |

---

## 5. Recommended Pin Assignments by Function (UFQFPN32 filtered)

> **Pins marked ✕ are NOT available on UFQFPN32.** Options using these pins are for
> LQFP48+ packages only.

### I2C Bus

| Option | SCL | SDA | Notes |
|--------|-----|-----|-------|
| **I2C1 on A4/A5** (default) | PA15 (AF4) | PB7 (AF4) | SB2+SB3 ON. PA5/PA6 must float |
| I2C1 on PB6/PB7 | PB6 (AF4) | PB7 (AF4) | Blocks USART1_TX/RX |
| I2C2 on PB10/PB11 ✕ | PB10 (AF4) | PB11 (AF4) | LQFP48+ only. Blocks USART3_TX/RX, SPI2 |
| I2C2 on PB13/PB11 ✕ | PB13 (AF4) | PB11 (AF4) | LQFP48+ only. Blocks USART3_CTS, SPI2 |
| I2C3 on PA9/PA10 | PA9 (AF4) | PA10 (AF4) | Blocks USART1_TX/RX |

### SPI Bus

| Option | SCK | MISO | MOSI | NSS | Notes |
|--------|-----|------|------|-----|-------|
| **SPI1 (default)** | PB3 (AF5) | PB4 (AF5) | PB5 (AF5) | PA4 or PA15 (AF5) | PB3=SWO (SB4 ON); PB5 has LED |
| SPI1 alt | PA5 (AF5) | PA6 (AF5) | PA7 (AF5) | PA4 (AF5) | Conflicts with SB2/SB3 side effects |
| SPI2 ✕ | PB10 (AF5) | PB14 (AF5) | PB15 (AF5) | PB12 (AF5) | LQFP48+ only. Blocks USART3 |
| SPI3 ✕ | PB3 (AF6) | PB4 (AF6) | PC12 (AF6) | PA15 (AF6) | LQFP48+ only (PC12 not on 32-pin) |

### USART Bus

| Option | TX | RX | CTS/RTS | Notes |
|--------|----|----|---------|-------|
| **USART1** | PA9 (AF7) | PA10 (AF7) | PA11/PA12 (AF7) | Available on CN3 TX/RX pins |
| **USART2 (VCP)** | PA2 (AF7) | PA3 (AF7) | PA0/PA1 (AF7) | Default VCP. Cut SB1/SB12 to free PA2/PA3 |
| USART3 ✕ | PB10 (AF7) | PB11 (AF7) | PB13/PB1 (AF7) | LQFP48+ only. Blocks I2C2 and SPI2 |

### CAN Bus (FDCAN1)

| Option | TX | RX | Notes |
|--------|----|----|-------|
| **FDCAN1 (CubeMX default)** | PA12 (AF9) | PA11 (AF9) | Verified on G431KB. PA11/PA12 shared with USB |
| FDCAN1 (datasheet remap) ✕ | PA8 (AF10) | PA9 (AF10) | Not verified on G431KB. PA9 conflicts with USART1_TX |
| FDCAN1 ✕ | PB9 (AF9) | PB11 (AF9) | LQFP48+ only. PB9 conflicts with I2C1_SDA |
| FDCAN1 ✕ | PB10 (AF9) | PB12 (AF9) | LQFP48+ only. PB10 conflicts with I2C2_SCL |
| FDCAN1 ✕ | PB8 (AF10) | PB9 (AF10) | LQFP48+ only. Remap option |

### Timer PWM (UFQFPN32 filtered)

> Pins marked ✕ are not available on UFQFPN32.

| Timer | CH1 | CH2 | CH3 | CH4 | Notes |
|-------|-----|-----|-----|-----|-------|
| TIM1 | PA8 | PA9 | PA10 | PA11 | Advanced timer, complementary outputs on PB13/14/15 ✕ |
| TIM2 | PA0/PA5/PA15 | PA1/PB3 | PA2/PB10 ✕ | PA3/PB11 ✕ | 32-bit timer |
| TIM3 | PA6/PB4/PC6 ✕ | PA7/PB5/PC7 ✕ | PB0/PC8 ✕ | PB1 ✕/PC9 ✕ | |
| TIM4 | PB6 | PB7 | PB8 | PB9 ✕ | General purpose |

---

## 6. Quick Conflict Reference (UFQFPN32)

### Pins you CAN use freely (no board conflicts)

| Pin | Best Functions |
|-----|---------------|
| PA0 | ADC, TIM2_CH1, USART2_CTS |
| PA1 | ADC, TIM2_CH2, LPUART1_RTS_DE |
| PA4 | ADC, SPI1_NSS, DAC1_OUT1 |
| PA7 | SPI1_MOSI, TIM3_CH2 |
| PA8 | TIM1_CH1, D9 |
| PA11 | FDCAN1_RX, D10 |
| PA12 | FDCAN1_TX |
| PB0 | ADC, TIM3_CH3 |
| PB4 | SPI1_MISO, TIM3_CH1, D3/D12 |
| PB6 | USART1_TX, TIM4_CH1, D6 |
| PB8 | TIM4_CH3, I2C1_SCL (if SB8 not needed), FDCAN1_TX remap |

### Pins with board-level restrictions

| Pin | Restriction |
|-----|-------------|
| PA2 | SB1 default ON (VCP TX). Cut SB1 to use freely |
| PA3 | SB12 default ON (VCP RX). Cut SB12 to use freely |
| PA5 | **Must float** when SB2=ON (I2C on A4) |
| PA6 | **Must float** when SB3=ON (I2C on A5) |
| PA13 | **SWDIO** — never reconfigure |
| PA14 | **SWCLK** — never reconfigure |
| PA15 | SB3 default ON (I2C SCL on A5). Cut SB3 to use as D5 |
| PB3 | SB4 default ON (SWO trace). Cut SB4 to use freely |
| PB5 | User LED (LD2) active low — available but has LED |
| PB7 | SB2 default ON (I2C SDA on A4). Cut SB2 to use as D4 |
| PF0 | SB8 default OFF. Only on D7 if SB8 soldered. OSC_IN |
| PF1 | SB11 default OFF. Only on D8 if SB11 soldered. OSC_OUT |

### Pins NOT available on UFQFPN32

| Pin | Reason |
|-----|--------|
| PB1 | Not bonded out on 32-pin package |
| PB2 | Not bonded out on 32-pin package |
| PB9–PB15 | Not bonded out on 32-pin package |
| PC0–PC15 | Not bonded out on 32-pin package |
| PF2, PF9, PF10 | Not bonded out on 32-pin package |
| PG10 | NRST — reset pin, not GPIO |
| VDD/VSS/VDDA | Power pins |

---

## 7. Important Notes

1. **⚠️ UFQFPN32 package constraint (STM32G431KB).** Only PB0, PB3–PB8 are bonded out on the 32-pin package. **PB1, PB2, PB9–PB15, PC0–PC15, and PF2/PF9/PF10 do NOT exist.** The AF tables and conflict matrices include these pins for completeness (useful when designing for LQFP48+ packages), but they are NOT usable on the G431KB. Always check the ✕ markers.

2. **Every pin can only serve ONE alternate function at a time.** Selecting AF4 for I2C1 on a pin precludes all other AFs on that same pin.

3. **I2C requires open-drain mode** with pull-ups. All other digital AFs typically use push-pull mode.

4. **⚠️ FDCAN1 AF discrepancy.** The datasheet (DS12589 Table 13) AF table lists FDCAN1_TX on PA8 (AF10) and FDCAN1_RX on PA9 (AF10). However, **CubeMX for the G431KB only offers FDCAN1 on PA11 (RX) / PA12 (TX) as AF9**. This is confirmed by ST Community reports. If your design requires FDCAN1, use PA11/PA12 — do not rely on the PA8/PA9 entries in the AF table.

5. **PF0/PF1 are oscillator pins.** If you need an external crystal, you cannot use these as GPIO. On the NUCLEO board, they are only routed to Arduino pins D7/D8 when SB8/SB11 are soldered.

6. **PA5 and PA6 have hidden restrictions** on the NUCLEO board due to the I2C solder bridges (SB2/SB3). Always check SB state before using these pins.

7. **USART1 TX/RX labels are swapped** on the NUCLEO CN3 connector (CN3 pin 1 = TX label = PA10 = USART1_RX, CN3 pin 2 = RX label = PA9 = USART1_TX).

8. **CN3/CN4 connector labels** on the NUCLEO-G431KB are swapped compared to other Nucleo-32 boards — this is a known design quirk.

9. **PA11/PA12 dual use.** These pins serve as both FDCAN1 (AF9) and USB (AF10). You cannot use FDCAN1 and USB simultaneously. CubeMX defaults to FDCAN1 when the FDCAN peripheral is enabled.

10. **FDCAN1 clock source.** The NUCLEO board has no dedicated CAN crystal. For accurate bit timing, solder SB13 to route ST-LINK MCO (25 MHz) to HSE, or use HSI16 with auto-calibration.

11. **DAC outputs** (PA4/PA5/PA6) are analog-only — they cannot be used as digital GPIO when the DAC is enabled on those channels.
