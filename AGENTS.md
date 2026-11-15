# ebike_ctrl — Agent Notes

The repo is grouped by **board**, then **MCU generation**. There is no top-level build;
work happens inside one firmware project.

```
boards/
  motherboard/f103/   firmware/ + hardware/   legacy, frozen
  motherboard/g431/   (Phase 2, not started)
  ui/f103/            firmware/ + hardware/   legacy
  ui/g431/            firmware/ + hardware/   current
protocol/             bike_can_protocol.h
tools/common.mk       shared LRR/protocol paths
docs/                 specs, datasheets, images
hardware/lib/         shared KiCad libraries
```

## Legacy (STM32F103C8, Cortex-M3)

- `boards/motherboard/f103/firmware/` — battery voltage, temps (KTY8x), current; telemetry over CAN
- `boards/ui/f103/firmware/` — receives CAN, renders to 16x2 HD44780 LCD
- `protocol/bike_can_protocol.h` — CAN wire format shared by **both** sides; changing it affects both firmwares and both test suites
- `boards/*/f103/hardware/` — KiCad projects

### Dependencies

- [l_rr library](https://github.com/rrowniak/l_rr): `tools/common.mk` defaults to
  `$(HOME)/dev/rrowniak.github.com/l_rr`. Override with `make LRR_PATH=/path/to/l_rr`.
  Needed even for host tests.
- Cross-toolchain `arm-none-eabi-gcc` on PATH, or pass `GCC_PATH=`.
- Host tests need Boost.Test (`libboost-test-dev`) and g++ with C++17.

Fix missing paths via `LRR_PATH` / `GCC_PATH` on the make command line, not by editing
`tools/common.mk` or the generated Makefiles.

### Commands

From a firmware dir (`boards/{motherboard,ui}/f103/firmware`):

- Build: `./build.sh` (just `make -j4`) → `build/firmware.{elf,hex,bin}`
- Tests: `make` in `tests/`, then run `./test`. Tests compile on the **host** with plain
  g++ using l_rr's `stm32_drv_fake`:
  - Motherboard tests inspect frames pushed onto the fake CAN buffer (`GetCanBusBuffer()`).
  - UI tests assert literal LCD lines (`hd44780_get_line*()`), so display changes intentionally break them.
- Flash: `./deploy.sh` → `sudo stm32flash -w build/firmware.hex -v -g 0x0 /dev/ttyUSB0`.

### Gotchas

- CubeMX-generated projects (`firmware.ioc`). Don't hand-edit generated regions of
  `main.c`, `_msp.c`, IT handlers, or anything under `Drivers/`.
- New sources must be added to the firmware `Makefile` `C_SOURCES` and, if tested, to
  `tests/Makefile`.
- UI links with `-u _printf_float`; Motherboard does not.
- `Makefile.inc` only locates `tools/common.mk`; put shared build config there.

## New project (STM32G431KB)

See [docs/HARDWARE_UPGRADE.md](./docs/HARDWARE_UPGRADE.md).

- `boards/ui/g431/firmware/` — current UI; EA DOGXL240W-7 (240x128) over SPI. No tests yet.
- `boards/motherboard/g431/` — Phase 2, does not exist yet.
- `protocol/bike_can_protocol.h` — same wire format until the G431 pair diverges.
- `boards/ui/g431/hardware/` — KiCad schematic; `boards/motherboard/f103/hardware/` is the legacy PCB.

The G431 project uses CubeMX's `Core/Inc` + `Core/Src` layout; the legacy projects keep
`Inc/` + `Src/`. Don't rename either just for symmetry.
