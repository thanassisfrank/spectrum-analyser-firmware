#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "esp_err.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "driver/gpio_filter.h"

#include "input.h"

static const char* TAG = "input";

button_t up_btn, right_btn, down_btn, left_btn, middle_btn;

// task for handling button interrupts
static QueueHandle_t button_intr_queue = NULL;
static TaskHandle_t button_intr_task_handle = NULL;

// output queue for input events (clicks)
static QueueHandle_t input_evt_queue = NULL;


// external function to read the queue
BaseType_t receive_input_event_queue(button_direction_t* btn_dir, TickType_t ticks_to_wait)
{
    return xQueueReceive(input_evt_queue, btn_dir, ticks_to_wait);
}


// config a button input pin
// adds internal pullup resistor and interrupt on edge
esp_err_t config_button_pin(gpio_num_t pin)
{
    // config structure
    const gpio_config_t pin_config = {
        .pin_bit_mask = 1 << pin,
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_NEGEDGE
    };

    return gpio_config(&pin_config);

    // const gpio_pin_glitch_filter_config_t pin_glitch_filter_config = {
    //     .clk_src = GLITCH_FILTER_CLK_SRC_DEFAULT,
    //     .gpio_num = pin
    // };

    // gpio_glitch_filter_handle_t glitch_filter_handle;

    // gpio_new_pin_glitch_filter(&pin_glitch_filter_config, &glitch_filter_handle);

    // return gpio_glitch_filter_enable(glitch_filter_handle);

    // TODO: store glitch filter handles to allow disabling and deletion later
}


// isr to enqueue interrupt events
static void IRAM_ATTR button_intr_handler(void* arg)
{
    button_t* btn = (button_t*) arg;
    // disable interrupts for this button
    gpio_intr_disable(btn->pin);
    xQueueSendFromISR(button_intr_queue, btn, NULL);
}



// freertos task to handle button interrupts
// takes button interrupt queue as input and input event queue as output
// TODO: implements software debouncing for each button
static void button_intr_task()
{
    button_t btn;
    for (;;) {
        if (xQueueReceive(button_intr_queue, &btn, portMAX_DELAY)) {
            // ESP_LOGI(TAG, "button edge");
            // wait for the debounce period
            vTaskDelay(pdMS_TO_TICKS(BTN_DEBOUNCE_MS));
            // read the value of the button
            if (gpio_get_level(btn.pin) == 0) {
                // if button is still low, register a real press event
                xQueueSend(input_evt_queue, &btn.btn_direction, 0);
            }
            // enable the interrupt again
            gpio_intr_enable(btn.pin);
        }
    }
}


// sets up the pins for the device button inputs
// accepts a callback to be run when a click is detected on any button
void setup_button_input_pins(input_pins_t input_pins) {
    // create the needed queues
    button_intr_queue = xQueueCreate(8, sizeof(button_t));
    input_evt_queue = xQueueCreate(8, sizeof(button_direction_t));
    // start the task to consume button click events
    xTaskCreate(button_intr_task, "button_intr_task", 2048, NULL, 10, &button_intr_task_handle);

    // configure the input gpio pins
    ESP_ERROR_CHECK(config_button_pin(input_pins.left));
    ESP_ERROR_CHECK(config_button_pin(input_pins.up));
    ESP_ERROR_CHECK(config_button_pin(input_pins.right));
    ESP_ERROR_CHECK(config_button_pin(input_pins.down));
    ESP_ERROR_CHECK(config_button_pin(input_pins.middle));

    ESP_ERROR_CHECK(gpio_install_isr_service(0));

    up_btn = (button_t){.btn_direction = BTN_UP, .pin = input_pins.up};
    right_btn = (button_t){.btn_direction = BTN_RIGHT, .pin = input_pins.right};
    down_btn = (button_t){.btn_direction = BTN_DOWN, .pin = input_pins.down};
    left_btn = (button_t){.btn_direction = BTN_LEFT, .pin = input_pins.left};
    middle_btn = (button_t){.btn_direction = BTN_MIDDLE, .pin = input_pins.middle};

    ESP_ERROR_CHECK(gpio_isr_handler_add(input_pins.up,     button_intr_handler, (void*) &up_btn));

    ESP_ERROR_CHECK(gpio_isr_handler_add(input_pins.right,  button_intr_handler, (void*) &right_btn));

    ESP_ERROR_CHECK(gpio_isr_handler_add(input_pins.down,   button_intr_handler, (void*) &down_btn));

    ESP_ERROR_CHECK(gpio_isr_handler_add(input_pins.left,   button_intr_handler, (void*) &left_btn));

    ESP_ERROR_CHECK(gpio_isr_handler_add(input_pins.middle, button_intr_handler, (void*) &middle_btn));
}