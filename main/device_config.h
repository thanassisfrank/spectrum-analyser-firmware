// contains configuration for the physical device
#pragma once

#include "driver/gpio.h"
#include "esp_adc/adc_oneshot.h"
#include "u8g2.h"
#include "display.h"


// pin assignments ============================================================

// for all SPI comms, the SPI2 controller is usd
// SPI signal wires
#define APP_MOSI_PIN GPIO_NUM_4
#define APP_MISO_PIN GPIO_NUM_NC
#define APP_SCLK_PIN GPIO_NUM_5

// chip selects for SPI
#define APP_RX_CS_PIN GPIO_NUM_6
#define APP_DISPLAY_CS_PIN GPIO_NUM_7
#define APP_SD_CS_PIN GPIO_NUM_NC

// display pins
#define APP_DISPLAY_DC_PIN GPIO_NUM_8
#define APP_DISPLAY_RST_PIN GPIO_NUM_2

// analog read pin for RSSI
#define APP_RSSI_ADC_CHANNEL ADC_CHANNEL_0

// button inputs
#define APP_UP_BTN_PIN GPIO_NUM_20
#define APP_RIGHT_BTN_PIN GPIO_NUM_3
#define APP_DOWN_BTN_PIN GPIO_NUM_9
#define APP_LEFT_BTN_PIN GPIO_NUM_21
#define APP_MIDDLE_BTN_PIN GPIO_NUM_1


// display config =============================================================

#define APP_DISPLAY_DRIVER DRIVER_SH1106
#define APP_DISPLAY_DEFAULT_ROTATION U8G2_R2


// adc config =================================================================

#define APP_ADC_ATTEN ADC_ATTEN_DB_2_5
#define APP_ADC_BITWIDTH ADC_BITWIDTH_DEFAULT
