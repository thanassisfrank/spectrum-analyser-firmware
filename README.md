# 5.8GHz Spectrum analyser Firmware

This is an open source project to create a device for analysing the 5.8GHz radio band using the RX5808 analog receiver module and the ESP32-C3 Zero. This repository holds the firmware for the device written with the ESP-IDF C SDK. For an overview of the hardware, please see *hardware repo*.

## External Libraries
There are a few external dependencies that are needed to build this program, the default search location for these are in `C:/{library name}` on Windows. This can be changed by modifying the `CMakeLists.txt` files within the individual component directories.

### [u8g2](https://github.com/olikraus/u8g2)

### [Lua](https://www.lua.org/)


## Display
The primary source of feedback for the user of the device is via a display. This is assumed to be a 128x64 monochrome display and an OLED with SSD1306 driver was tested during development. For graphics calls, the *u8g2* library is used. with a Hardware Abstraction Layer (HAL) created to map to the ESP-IDF LCD and SPI commands.

## Input
User input is taken in the form of momentary switches. There are five different buttons accounted for in the program that are assumed to be in a cross pattern, each used for navigation around the UI.

## (WIP) LUA API
An eventual goal of this project is to allow programs to be written and loaded as scripts using the LUA language in a similar way to OpenTX/EdgeTX. An API will be supplied to interface with all aspects of the device.


## (WIP) Programs

### Spectrum sweep
Displays a bar graph giving an overview of the entire spectrum at once. Potentially highlights the highest/lowest frequency points.

### Channel monitor
Allows cycling through the supported bands and displays the power on each channel within. Marks the best/worst channels in each band.

### Drone finder
Continually reads the rssi for a chosen band and channel, displays rssi changing to allow direction finding in combination with a directional antenna.

### Lap timer
Monitors a given set of channel/bands corresponsing to the participants in a race. Monitors the time between the rssi peaking which coresponds to lap times.

### Settings
Allows modifications to be made to the device operation at runtime. Changes persist when rebooting.
