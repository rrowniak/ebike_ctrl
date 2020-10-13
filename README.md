# ebike_ctrl
A small but powerful on-board computer displaying main parameters of almost any electric bike.
It concists of three parts:
* Motherboard - sits close to the BLDC driver and battery. Main responsibility is to gather all needed data like battery voltage, temperatures, current, hall sensor output and so on. The motherboard communicates with UI module via CAN bus.
* Current sensor. This is a hardware module (no firmware needed) connected to the motherboard.
* UI - displays all valuable info to a user. It reads what the motherboard is sending via CAN bus, then interprets the data, does additional calculations and displays on 16x2 LCD.

## Features

* Displays current up to +/- 100A, voltage up to 100V and battery percentage

* Current speed, distance traveled (three profiles)

* Consumption in Wh, Wh/km +gain from regenerative charging

* Temperatures: ambient, motor, battery, driver

* Allows for changing views

* and more

## Requirements and dependencies

* GNU Arm Embedded Toolchain

* [l_rr library](https://github.com/rrowniak/l_rr)

## License

Copyright (c) 2020 Rafal Rowniak rrowniak.com
