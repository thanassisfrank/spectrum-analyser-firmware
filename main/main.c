// main.c
// contains the entrypoint
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "esp_err.h"
#include "esp_log.h"
#include "driver/spi_master.h"
#include "esp_adc/adc_oneshot.h"
#include "driver/gpio.h"

#include "u8g2.h"
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#include "ws2812.h"

#include "device_config.h"
#include "app.h"

#include "display.h"
#include "gui.h"
#include "receiver.h"
#include "input.h"

static const char* TAG = "main";

static app_state_t app_state;

static ws2812_t status_led;

static receiver_device_t receiver;

static adc_oneshot_unit_handle_t adc1_handle;


void setup_spi_bus(gpio_num_t sclk_pin, gpio_num_t mosi_pin, gpio_num_t miso_pin)
{
     spi_bus_config_t buscfg = {
        .sclk_io_num = sclk_pin,
        .mosi_io_num = mosi_pin,
        .miso_io_num = miso_pin,
        .quadwp_io_num = -1, // not QSPI
        .quadhd_io_num = -1, // not QSPI
    };
    ESP_ERROR_CHECK(spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_CH_AUTO)); // Enable the DMA feature
}


// initialise adc1 and 
void setup_adc()
{
    // init adc unit
    adc_oneshot_unit_init_cfg_t init_config1 = {
        .unit_id = ADC_UNIT_1,
        .ulp_mode = ADC_ULP_MODE_DISABLE,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config1, &adc1_handle));

    // setup the adc on the needed channel (pin)
    adc_oneshot_chan_cfg_t config = {
        .bitwidth = ADC_BITWIDTH_DEFAULT,
        .atten = ADC_ATTEN_DB_0,
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, APP_RSSI_ADC_CHANNEL, &config));
    
}

// click handler
void app_click_handler(button_direction_t btn_dir)
{
    switch (btn_dir)
    {
        case BTN_LEFT:
            ESP_LOGI(TAG, "left");
        break;
        case BTN_UP:
            ESP_LOGI(TAG, "up");
        break;
        case BTN_RIGHT:
            ESP_LOGI(TAG, "right");
        break;
        case BTN_DOWN:
            ESP_LOGI(TAG, "down");
        break;
        case BTN_MIDDLE:
            ESP_LOGI(TAG, "middle");
        break;
    }
}

// all the operations for startup
esp_err_t app_load()
{
    app_state.current_screen = SCREEN_NONE;
    
    // setup the adc
    setup_adc();
    
    // setup the common spi bus
    setup_spi_bus(APP_SCLK_PIN, APP_MOSI_PIN, APP_MISO_PIN);

    // setup the display
    spi_display_pins_t display_pins = {
        .clk = APP_SCLK_PIN, 
        .mosi = APP_MOSI_PIN, 
        .miso = APP_MISO_PIN,
        .cs = APP_DISPLAY_CS_PIN,
        .dc = APP_DISPLAY_DC_PIN,
        .rst = APP_DISPLAY_RST_PIN
    };
    ESP_ERROR_CHECK(setup_display(&app_state.u8g2, APP_DISPLAY_DRIVER, APP_DISPLAY_DEFAULT_ROTATION, display_pins));

    init_display(&app_state.u8g2);

    // display the splash screen
    gui_draw_splash(&app_state.u8g2);

    // setup the receiver module
    spi_receiver_pins_t rx_pins = {
        .clk = APP_SCLK_PIN, 
        .mosi = APP_MOSI_PIN, 
        .cs = APP_RX_CS_PIN
    };
    setup_receiver(&receiver, rx_pins, adc1_handle, APP_RSSI_ADC_CHANNEL);

    // setup the lua environment
    app_state.L = luaL_newstate();


    // setup the button event interrupts
    input_pins_t input_pins = {
        .left = APP_LEFT_BTN_PIN,
        .up = APP_UP_BTN_PIN,
        .right = APP_RIGHT_BTN_PIN,
        .down = APP_DOWN_BTN_PIN,
        .middle = APP_MIDDLE_BTN_PIN,
    };
    setup_button_input_pins(input_pins, app_click_handler);

    return ESP_OK;
}



void app_main(void)
{
    status_led = create_ws2812(GPIO_NUM_10);
    set_color_ws2812(&status_led, WS2812_DIM_BLUE);
    ESP_ERROR_CHECK(app_load());
    set_color_ws2812(&status_led, WS2812_DIM_GREEN);

    set_receiver_freq_mhz(&receiver, 5800);

    while (1)
    {
        ESP_LOGI(TAG, "reading: %i", get_receiver_rssi(&receiver));
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

}