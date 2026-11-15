# Old UI Firmware — Feature Analysis

Reference document for porting the old STM32F103C8 UI firmware to the new STM32G431KB platform.

## CAN Data Received

Three message types defined in `include/bike_can_protocol.h`, each 7 bytes with byte 0 = message type ID.

| Message ID | Struct | Fields |
|---|---|---|
| `BCP_MSG_ELECTRIC` (0x01) | `bcp_msg_electric` | `timestamp:13`, `voltage:10` (dV), `current:14` (signed, ÷10), `faults:11`, `seq_id:8` |
| `BCP_MSG_MOTION` (0x02) | `bcp_msg_motion` | `timestamp:13`, `tot_pulses:32`, `seq_id:8`, `reserved:2` |
| `BCP_MSG_SENS_BLK1` (0x03) | `bcp_msg_sens_blk1` | `moto_t:9` (signed), `drv_t:9` (signed), `batt_t:9` (signed), `reserved:29` |

### Field Details

- **Voltage**: 10-bit, in decivolts — divide by 10 to get Volts.
- **Current**: 14-bit signed, ÷10 for Amps. Sign indicates direction (positive = discharge, negative = regen). Inverted if `reverse_curr` config flag is set.
- **Faults**: 11-bit, received from motherboard but **not displayed** in old UI.
- **Pulses**: 32-bit cumulative wheel pulses — used for speed and distance calculations.
- **Timestamps**: 13-bit, wraps at `MAX_TIMESTAMP = 0x2000 = 8192ms`. Delta between consecutive timestamps used for speed calculation.
- **Temperatures**: 9-bit signed, from KTY8x sensors on motor, driver, and battery.

## Calculated Values

| Value | Formula | Unit |
|---|---|---|
| Speed | `36 * delta_mm / (10 * delta_t_ms)` | km/h |
| Battery % | Linear interpolation between `cell_mv_min * batt_s` and `cell_mv_max * batt_s` | % |
| Total distance | `total_pulses * dist_p_rev_mm / pulse_p_rev / 1e6` | km |
| Trip1/Trip2 distance | Offset from total at last reset point | km |
| Consumed Wh | Accumulated `abs(current) * voltage * dt`, integrated every 10s | Wh |
| Regen brake Wh | Accumulated when current is negative (regen), integrated every 10s | Wh |
| Wh/km | `(consumed_Wh - brake_Wh) / total_km` | Wh/km |
| Instant watts | `abs(current) * voltage` | W |

### Amp Sanity Check

On first current reading: if `abs(current) > 30A`, the firmware assumes the current sensor is faulty and disables amp-related display (replaces with temperature mode on bottom line).

## Display — 6 Modes

16×2 HD44780 LCD. Display mode cycled by short-pressing BTN3.

| Mode | Top Line (Line 0) | Bottom Line (Line 1) |
|---|---|---|
| `DM_DEFAULT` | `{speed} km/h {total_km}` | `{batt_v}V {batt_perc}% {current}A` |
| `DM_TRIP1` | `{speed} km/h {trip1_km}-1` | same as DM_DEFAULT |
| `DM_TRIP2` | `{speed} km/h {trip2_km}-2` | same as DM_DEFAULT |
| `DM_TEMP` | `{amb}C {moto}C {drv}C {batt}C` | same as DM_DEFAULT |
| `DM_POWER` | `- {consumed_Wh} +{brake_Wh}` | same as DM_DEFAULT |
| `DM_POWER2` | `{watts}W {wh_per_km}Wh/km` | same as DM_DEFAULT |
| Offline | `OFFLINE!` | `OFFLINE!` |

### Formatting

- Right-aligned numeric values with unit suffix character
- Auto-scaling with `k` prefix for values >= 10000
- Negative temperatures clamped to 0 for display
- Regen braking shown as `+` prefix in Power2 mode
- Trip distances suffixed with `-1` or `-2`

### Welcome Screen

Two sequential screens shown at boot (700ms each):
1. "Rafal Rowniak / rrowniak.com" + beep
2. "BOROWY / ver: 1.0" + beep

## Button Handling

3 buttons (S1=PB5, S2=PB4, S3=PB3), active-low with internal pull-ups. Debounced via EXTI interrupts (rising+falling edge detection).

| Action | Button | Duration | Effect |
|---|---|---|---|
| Short press | BTN3 | < 1s | Cycle display mode (DM_DEFAULT → DM_TRIP1 → DM_TRIP2 → DM_TEMP → DM_POWER → DM_POWER2 → wrap) |
| Long press | BTN3 | 1s | Toggle LCD backlight |
| Long press | BTN1 | 1s | Reset trip 1 |
| Long press | BTN2 | 1s | Reset trip 2 |
| Held at boot | BTN1 + BTN2 | > 0 at startup | Enter initial configuration wizard |

## Thresholds & Alarms

| Threshold | Condition | Response |
|---|---|---|
| Amp sanity | First current reading > 30A | Disable amp display, assume sensor fault |
| Motherboard offline | No CAN message for > 3.5s | Display "OFFLINE!" on both lines |
| Inactivity save | Speed == 0 for 1s after any movement | Save runtime to EEPROM |
| Inactivity beep | Stationary for 60s | 60 beeps (1 beep/s for 1 minute) |

### Temperature Alarm Thresholds (stored in config, not acted on in display)

- `moto_t_alarm_c`: 25–150°C (default 90°C)
- `batt_t_alarm_c`: 25–80°C (default 60°C)
- `drv_t_alarm_c`: 25–150°C (default 90°C)

## Persistence — EEPROM (I2C 24LC256)

### EEPROM Layout (1KB pages)

| Page Address | Content |
|---|---|
| `0x0000` | `eeprom_constants` — magic number `0xabd131`, firmware version string |
| `0x0400` | `vehicle_conf` — battery config, sensor params, alarm thresholds |
| `0x0800` | `vehicle_runtime` — trips, total distance, display mode, charge cycles, energy |
| `0x0C00` | `faults` — reserved (not used in old UI) |

### vehicle_conf Fields

| Field | Type | Default | Range |
|---|---|---|---|
| `batt_s` | uint16 | 20 | 1–250 (series cell count) |
| `batt_p` | uint16 | 17 | 1–250 (parallel cell count) |
| `cell_cap_mah` | uint16 | 2850 | 1000–4000 mAh, step 50 |
| `cell_mv_max` | uint16 | 4200 | 3000–5000 mV, step 100 |
| `cell_mv_min` | uint16 | 3200 | 2500–4000 mV, step 100 |
| `reverse_curr` | uint8 | 0 | 0 or 1 |
| `pulse_p_rev` | uint16 | 16 | 1–500 |
| `dist_p_rev_mm` | uint16 | 1830 | 1000–3000 mm, step 1 |
| `moto_t_alarm_c` | uint8 | 90 | 25–150°C |
| `batt_t_alarm_c` | uint8 | 60 | 25–80°C |
| `drv_t_alarm_c` | uint8 | 90 | 25–150°C |

### vehicle_runtime Fields

| Field | Type | Description |
|---|---|---|
| `last_batt_mv` | uint32 | Last known battery millivolts |
| `pow_consumed_mah` | uint32 | Total consumed mAh (lifetime) |
| `full_batt_charge_cycles` | uint32 | Full charge cycle counter |
| `current_display_mode` | uint8 | Last active display mode |
| `total` | trip_runtime | Total distance and stats |
| `trip1` | trip_runtime | Trip 1 distance and stats |
| `trip2` | trip_runtime | Trip 2 distance and stats |

### trip_runtime Fields

| Field | Type | Description |
|---|---|---|
| `dist_pulses` | uint32 | Distance in pulses |
| `travel_time_s` | uint32 | Travel time in seconds |
| `max_speed_kmh` | uint32 | Maximum speed recorded |
| `consumed_mah` | uint32 | mAh consumed in this trip |

### Save Triggers

- **Auto-save**: 1 second after vehicle stops (speed transitions to 0 after any movement)
- **Manual save**: After completing the initial configuration wizard

## Temperature Sensing

- Onboard KTY81-120 thermistor connected to ADC1 channel 9 (PB1)
- Voltage divider: 2200Ω pull-up to 5V
- 3-sample ADC averaging with 3ms delay between samples
- +0.2V error correction offset applied to ADC voltage
- Re-read every 30 seconds
- Conversion via `kty81_120_get_temp(rt)` from l_rr library

## Initial Configuration Wizard

Triggered by holding BTN1+BTN2 at boot. Uses BTN1=decrement, BTN2=increment, BTN3=confirm/next.

| # | Parameter | Min | Max | Step |
|---|---|---|---|---|
| 1 | Battery S (series) | 1 | 250 | 1 |
| 2 | Battery P (parallel) | 1 | 250 | 1 |
| 3 | Cell capacity | 1000 | 4000 mAh | 50 |
| 4 | Cell max voltage | 3000 | 5000 mV | 100 |
| 5 | Cell min voltage | 2500 | 4000 mV | 100 |
| 6 | Reverse current | 0 | 1 | 1 |
| 7 | Pulses per revolution | 1 | 500 | 1 |
| 8 | Distance per revolution | 1000 | 3000 mm | 1 |
| 9 | Motor alarm temp | 25 | 150°C | 1 |
| 10 | Battery alarm temp | 25 | 80°C | 1 |
| 11 | Driver alarm temp | 25 | 150°C | 1 |

After completing all parameters, new configuration is saved to EEPROM.

## Error Handling

| Error | Behavior |
|---|---|
| CAN read failure | Log error, skip message, continue |
| EEPROM load failure | Log error, continue with defaults |
| ADC conversion failure | Return negative error code |
| `panic()` | Display error on LCD, halt system |
| Hard fault | Loop forever (standard STM32 fault handler) |

## Software Timer Summary

| Timer | Period | Actions |
|---|---|---|
| Main loop | Continuous | Drain CAN FIFO, process messages, reset watchdog |
| Button | 20ms | Debounce, detect long presses (50 tick threshold = 1s) |
| UI refresh | 500ms | Check motherboard watchdog, call `ui_update()` |
| Housekeeping | 1s | Increment watchdog, inactivity detection, EEPROM save |
| Energy accounting | 10s | Convert accumulated Ws → Wh, compute Wh/km |
| Ambient temp | 30s | Re-read onboard KTY81-120 sensor |
