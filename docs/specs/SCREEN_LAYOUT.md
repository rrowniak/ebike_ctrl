# This is a design of different screens

## Screens

- Welcome screen defined as `welcome_screen` function.
- Test screen. Only for debugging purposes. Will be removed from the final version.
- Main screen group — the primary riding screens (2 sub-pages).
- Custom screen group — non-riding screens accessed via button 1 from Main group:
  - BMS screen (4 sub-pages).
  - Faults / vehicle status screen (to be designed).
  - Configuration screen.

## Finite State Machine

### Boot sequence

Boot-up -> Welcome screen -> sleep 500 -> Test screen -> sleep 500 -> Main screen P1.

### Button 1 short press — cycle Main group and Custom

Button 1 cycles linearly through the top-level screen groups:

Main P1 → Main P2 → Custom → Main P1 → ...

Within the Custom group, button 1 returns to Main P1.

### Button 1 long press — lights toggle

- Long press button 1 toggles lights on/off (from any screen).
- When lights are ON: display a light icon on the Main screen.
- Lights state persists until next long press or power-off.

### Buttons 2 and 3 — navigation within Custom group

Within the Custom group, buttons 2/3 cycle through sub-screens:

BMS P1 → BMS P2 → BMS P3 → BMS P4 → Faults → Config → BMS P1 → ...

Button 2 = next, button 3 = previous.

### Configuration screen — edit mode

- Long press button 2 or 3 (on the selected field) to enter edit mode.
- In edit mode: button 2 = increment, button 3 = decrement.
- Button 1 = confirm changes and return to read-only mode.
- Long press button 2 or 3 = cancel edit, return to read-only mode.

## Welcome screen

Already implemented. Nothing to do so far. In the future we might consider adding some graphical effects or animations.

## Test screen

Displays some samples to prove that fonts and special symbols are implemented correctly. Bare minimum for all fonts is to display this: "0123456789.+-/%°CAWAhkmV".

## Main screen

The Main screen has 2 sub-pages, cycled with button 1 (linear: P1 → P2 → Custom).

### Page 1 — Primary

Mixed fonts (large/medium/small) + icons + graphical rules for a clean look.

**Status bar (top, small font + icons):**

- Icons (left): lights (when ON), fault (active fault), offline (no motherboard signal), alarm (temp alarm active)
- Page indicator (right): `1/2`
- Bordered below by a horizontal line

**Body** (split by a vertical line at ~2/3 width):

Left column (~2/3) — riding data:

- Speed — large font, centered
- Below a horizontal rule: instant watts (medium font) and ambient temperature (small font), separated by a short vertical tick

Right column (~1/3) — battery block:

- SOC % — large font, next to a battery icon
- Capacity bar (segmented graphical bar)
- Battery voltage — small font
- Estimated range — small font

```
+───────────────────────────────┬──────────────────────────────+
│  💡  ⚠  ⌁  ‼                 │                     · 1 / 2  │
├───────────────────────────────┼──────────────────────────────┤
│                               │     ██████████░░  ███        │
│           25.3                │            78 %              │
│           km/h                │     [████████░░░░]           │
│                               │                              │
│  ───────────────────────────  │    32.4 V        58 km       │
│   336 W     ·     21 °C       │                              │
└───────────────────────────────┴──────────────────────────────┘
```

Estimated range = remaining_capacity × avg_voltage / Wh_per_100km × 100. Display "--" if insufficient data.

Fonts: large = speed, SOC %; medium = instant watts, voltage; small = ambient temperature, range, units, status bar.

### Page 2 — Distances and efficiency

| Data | Unit |
|------|------|
| Distance — total / since start / trip | km |
| Consumed Wh — total / since start / trip | Wh |
| Regen Wh — total / since start / trip | Wh |
| Wh/100km — total / since start / trip | Wh/100km |
| Instant current | A |
| Motor temperature | °C |

### Challenges

- The ASCII mockup above is a proposal — confirm it reads well at real size before committing.
- Icon assets (lights, fault, offline, alarm) need to be defined.
- How to present the data in visually attractive way still being readable on 240×128 px.

## BMS screen

Part of the Custom group. Too much data for one screen — 4 sub-pages, cycled with buttons 2/3.

### Page 1 — Overview

| Data | Unit |
|------|------|
| Battery voltage | V |
| Battery current | A |
| Battery power (V × A) | W |
| SOC | % |
| Remaining capacity | Ah |
| Full capacity | Ah |
| Cycle count | cycles |
| Charge MOS | on/off |
| Discharge MOS | on/off |

### Page 2 — Cells (part 1)

| Data | Unit |
|------|------|
| Cell 01 voltage | V |
| Cell 02 voltage | V |
| ... | |
| Cell 10 voltage | V |
| Cell min voltage | V |
| Cell max voltage | V |
| Cell delta voltage | V |

### Page 3 — Cells (part 2)

| Data | Unit |
|------|------|
| Cell 11 voltage | V |
| Cell 12 voltage | V |
| ... | |
| Cell 20 voltage | V |
| Cell min voltage | V |
| Cell max voltage | V |
| Cell delta voltage | V |

### Page 4 — Temperatures and status

| Data | Unit |
|------|------|
| Temperature 1 | °C |
| Temperature 2 | °C |
| ... | |
| Balancing status | |
| Protection status | bitfield |

### Challenges

- Cell voltages may vary by BMS — pages 2/3 assume 20 cells. Adjust layout if cell count is configurable.
- Consider coloring/flagging cells outside voltage tolerance.

## Faults / vehicle status screen

Part of the Custom group. To be designed.

Placeholder — will display:

- Active fault codes from BMS and motor controller.
- Vehicle status (ready, fault, limiting, etc.).
- Connectivity status with the motherboard.

## Configuration screen

Part of the Custom group. Read and edit configuration stored in the EEPROM.

### Behaviour

**Read-only mode (default):**

- Display field names and current values in a list.
- With 11 fields, split across 2 pages (~6 fields per page).
- Buttons 2/3 move the cursor between fields (wrap within page).
- Long press button 2 or 3 to enter edit mode on the selected field.
- Button 1 returns to Main P1.

**Edit mode:**

- The currently selected field is highlighted (inverted or bordered).
- Button 2 = increment value (or next option for bool fields).
- Button 3 = decrement value (or previous option for bool fields).
- Long press button 2 or 3 = cancel edit and return to read-only mode.
- Button 1 = confirm changes and return to read-only mode.

**Saving:**

- Changes are written to EEPROM only on confirm (button 1 press).
- A brief "saving..." indicator may be shown.

### Fields

| # | Field | Type | Default | Range | Step | Description |
|---|---|---|---|---|---|---|
| 1 | `batt_s` | uint16 | 20 | 1–250 | 1 | Series cell count |
| 2 | `batt_p` | uint16 | 17 | 1–250 | 1 | Parallel cell count |
| 3 | `cell_cap_mah` | uint16 | 2850 | 1000–4000 | 50 | Cell capacity in mAh |
| 4 | `cell_mv_max` | uint16 | 4200 | 3000–5000 | 100 | Cell max voltage in mV |
| 5 | `cell_mv_min` | uint16 | 3200 | 2500–4000 | 100 | Cell min voltage in mV |
| 6 | `reverse_curr` | bool | 0 | 0 or 1 | — | Positive current treated as regen |
| 7 | `pulse_p_rev` | uint16 | 16 | 1–500 | 1 | BLDC pulses per full revolution |
| 8 | `dist_p_rev_mm` | uint16 | 1830 | 1000–3000 | 1 | Wheel circumference in mm |
| 9 | `moto_t_alarm_c` | uint8 | 90 | 25–150 | 1 | Motor temp alarm threshold (°C) |
| 10 | `batt_t_alarm_c` | uint8 | 60 | 25–80 | 1 | Battery temp alarm threshold (°C) |
| 11 | `drv_t_alarm_c` | uint8 | 90 | 25–150 | 1 | Driver temp alarm threshold (°C) |

### Page layout

**Page 1 (fields 1–6):**

```
batt_s:      20
batt_p:      17
cell_cap:    2850 mAh
cell_max:    4200 mV
cell_min:    3200 mV
rev_curr:    OFF
```

**Page 2 (fields 7–11):**

```
pulse/rev:   16
dist/rev:    1830 mm
moto_alarm:  90 °C
batt_alarm:  60 °C
drv_alarm:   90 °C
```

