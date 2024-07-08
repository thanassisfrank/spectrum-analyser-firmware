// display.h
#pragma once

#include "driver/gpio.h"

#include "u8g2.h"

typedef enum {
    DRIVER_NONE,
    DRIVER_SSD1305, 
    DRIVER_SSD1306, 
    DRIVER_SSD1309, 
    DRIVER_SSD1312, 
    DRIVER_SSD1316, 
    DRIVER_SSD1318,
    DRIVER_SSD1320, 
    DRIVER_SSD1322, 
    DRIVER_SSD1325, 
    DRIVER_SSD1327, 
    DRIVER_SSD1329, 
    DRIVER_SSD1606, 
    DRIVER_SSD1607, 
    DRIVER_SH1106, 
    DRIVER_SH1107, 
    DRIVER_SH1108, 
    DRIVER_SH1122, 
    DRIVER_T6963, 
    DRIVER_RA8835, 
    DRIVER_LC7981, 
    DRIVER_PCD8544,
    DRIVER_PCF8812,
    DRIVER_HX1230, 
    DRIVER_UC1601, 
    DRIVER_UC1604, 
    DRIVER_UC1608, 
    DRIVER_UC1610, 
    DRIVER_UC1611, 
    DRIVER_UC1617, 
    DRIVER_UC1638, 
    DRIVER_UC1701, 
    DRIVER_ST7511, 
    DRIVER_ST7528, 
    DRIVER_ST7565, 
    DRIVER_ST7567,
    DRIVER_ST7571, 
    DRIVER_ST7586, 
    DRIVER_ST7588, 
    DRIVER_ST75160, 
    DRIVER_ST75256, 
    DRIVER_ST75320,
    DRIVER_NT7534, 
    DRIVER_ST7920, 
    DRIVER_IST3020, 
    DRIVER_IST3088,
    DRIVER_IST7920,
    DRIVER_LD7032, 
    DRIVER_KS0108, 
    DRIVER_KS0713, 
    DRIVER_HD44102, 
    DRIVER_T7932, 
    DRIVER_SED1520, 
    DRIVER_SBN1661,
    DRIVER_IL3820, 
    DRIVER_MAX7219, 
    DRIVER_GP1287, 
    DRIVER_GP1247, 
    DRIVER_GU800
} display_driver_t;

typedef struct {
    gpio_num_t clk, miso, mosi, dc, cs, rst;
} spi_display_pins_t;

esp_err_t setup_display(u8g2_t*, display_driver_t, const u8g2_cb_t*, spi_display_pins_t);

void init_display(u8g2_t*);