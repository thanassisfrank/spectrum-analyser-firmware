# 5.8GHz Spectrum analyser Firmware

This is an open source project to create a device for analysing the 5.8GHz radio band using the RX5808 analog receiver module and the ESP32-C3 Zero. This repository holds the firmware for the device written with the ESP-IDF C SDK. For an overview of the hardware, please see *hardware repo*.

## External Libraries
There are a few external dependencies that are needed to build this program, the default search location for these are in `C:/{library name}` on Windows but this can be changed by modifying `main/CMakeLists.txt`.

### u8g2

### Lua


## RSSI

## Display
The primary source of feedback for the user of the device is via a display. This is assumed to be a 128x64 monochrome display and an OLED with SSD1306 driver was tested during development. For graphics calls, the *u8g2* library is used. with a Hardware Abstraction Layer (HAL) created to map to the ESP-IDF LCD and SPI commands.

## Input
User input is taken in the form of momentary switches. There are five different buttons accounted for in the program that are assumed to be in a cross pattern, each used for navigation around the UI.

## (WIP) LUA API
An eventual goal of this project is to allow programs to be written and loaded as scripts using the LUA language in a similar way to OpenTX/EdgeTX. An API will be supplied to interface with all aspects of the device.

### Spectrum sweep

### Lap timer
