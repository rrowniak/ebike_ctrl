# ebike_ctrl
A small controller of main parameters of BLDC powered ebike.
It concists of three parts:
* Motherboard - sits close to the BLDC driver and battery. Main responsibility is to gather all needed data like battery voltage, temperatures, current, hall sensor output and so on. The motherboard communicates with UI module via CAN bus.
* Current sensor. This is a hardware module (no firmware needed) connected to the motherboard.
* UI - displays all necessary info to a user. It reads what the motherboard is sending via CAN bus, then interprets the data, does additional calculation and displays on 16x2 LCD.

## Features

* Displays current up to +/- 100A, voltage up to 100V and battery percentage

* Current speed, distance traveled (three profiles)

* Consumption in Wh, Wh/km +gain from regenerative charging

* Temperatures: ambient, motor, battery, driver

* and more

## License

Copyright (c) 2020 Rafal Rowniak rrowniak.com
