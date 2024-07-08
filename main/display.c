// display.c

#include "esp_err.h"
#include "esp_log.h"
#include "driver/spi_master.h"

#include "u8g2.h"
#include "u8g2_esp32_hal.h"

#include "display.h"

// setup the display for the program in full screen mode, will create a new SPI device on SPI2
// accepts the type of driver chip to use
// returns the u8g2 handle of the newly created display
esp_err_t setup_display(u8g2_t* u8g2, display_driver_t driver, const u8g2_cb_t* rotation, spi_display_pins_t display_pins)
{
    u8g2_esp32_hal_t u8g2_esp32_hal = U8G2_ESP32_HAL_DEFAULT;
    u8g2_esp32_hal.bus.spi.clk = display_pins.clk;
    u8g2_esp32_hal.bus.spi.mosi = display_pins.mosi;
    u8g2_esp32_hal.bus.spi.cs = display_pins.cs;
    u8g2_esp32_hal.dc = display_pins.dc;
    u8g2_esp32_hal.reset = display_pins.rst;
    u8g2_esp32_hal_init(u8g2_esp32_hal);

    switch (driver) {
        case DRIVER_SSD1306:
            u8g2_Setup_ssd1306_128x64_noname_f(u8g2, rotation, u8g2_esp32_spi_byte_cb, u8g2_esp32_gpio_and_delay_cb);
            break;
        case DRIVER_SH1106:
            u8g2_Setup_sh1106_128x64_noname_f(u8g2, rotation, u8g2_esp32_spi_byte_cb, u8g2_esp32_gpio_and_delay_cb);
            break;
        default:
            return ESP_ERR_INVALID_ARG;
    };    
    return ESP_OK;  
}

void init_display(u8g2_t* u8g2) 
{
    u8g2_InitDisplay(u8g2);  // send init sequence to the display
    u8g2_SetPowerSave(u8g2, 0); // wake display
    u8g2_SetContrast(u8g2, 100); // set contrast to max
    u8g2_ClearBuffer(u8g2); // clear the sreen buffer
}