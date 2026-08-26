# Hardware Upgrade Plan

The ebike controller consists of two independent STM32 units — **Motherboard** and **UI**.
This document tracks the planned hardware upgrade, split into two phases.

## Phase 1 — UI Unit

Replaces the current STM32F103C8 + HD44780 16×2 LCD with modern components.

### Selected parts

| Component | Part | Role |
|---|---|---|
| MCU | **NUCLEO-G431KB** (STM32G431CB in UFQFPN32 package, 128 KB flash / 32 KB RAM) | Main controller, FDCAN, better ADC |
| Display | **EA DOGXL240W-7** (240×128 COG LCD, SPI) | Primary display — sunlight-readable |
| Temperature sensor | **LM35** (analog output, −55…+150 °C) | Ambient/display temperature monitoring |
| EEPROM | **24L256** (256 Kbit I²C) | Vehicle configuration storage |
| CAN transceiver | **MCP2562** (high-speed CAN) | CAN bus interface to Motherboard |

### Rationale

- **STM32G431CB** — active product (≥2036), FDCAN (CAN 2.0 compatible), 12-bit ADC with
  hardware oversampling, 32 KB RAM handles the new display framebuffer, same CubeMX/HAL
  workflow. See `MCU.md` for full analysis.
- **EA DOGXL240W-7** — 240×128 COG LCD with SPI, sunlight-readable, bare-metal driver
  friendly. Replaces the Sharp Memory LCD option from earlier discussions.
- **LM35** — simple analog temperature sensor, direct ADC read, no digital protocol needed.
- **24L256** — same I²C EEPROM family already used in the current UI firmware
  (`lrr_eeprom_24LC256`), straightforward port.
- **MCP2562** — CAN 2.0 high-speed transceiver; pairs with the G431's FDCAN peripheral.

### Key files

- `UI/firmware/` — the old UI firmware project (CubeMX-generated, `firmware.ioc`)
- `UI/firmware2/` — the new/current UI firmware project (CubeMX-generated, `firmware2.ioc`)
- `include/bike_can_protocol.h` — shared CAN wire format (both sides)

### Migration notes

- FDCAN HAL API differs from bxCAN — `Src/can.c` and its test fake must be reworked.
- Display driver is new; existing `ui.c` abstraction layer keeps app logic untouched.
- `l_rr` library drivers need porting to G4 HAL; matching `stm32_drv_fake` fakes too.
- Deploy path (UART bootloader via `deploy.sh`) is preserved on the G431.

## Phase 2 — Motherboard + BMS

Upgrade the Motherboard unit (and potentially the BMS) to the same MCU family.
TBD — details to be added once Phase 1 is complete.

## Timeline

| Phase | Scope | Status |
|---|---|---|
| Phase 1 | UI unit only | In progress |
| Phase 2 | Motherboard + BMS | Planned |
