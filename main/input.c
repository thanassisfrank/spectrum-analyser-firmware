#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "esp_err.h"
#include "esp_log.h"
#include "driver/gpio.h"

#include "input.h"

static const char* TAG = "input";

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
}


// isr to enqueue interrupt events
static void IRAM_ATTR button_intr_handler(void* arg)
{
    button_direction_t btn_dir = (button_direction_t) arg;
    xQueueSendFromISR(button_intr_queue, &btn_dir, NULL);
}


// freertos task to handle button interrupts
// takes button interrupt queue as input and input event queue as output
// TODO: implements software debouncing for each button
static void button_intr_task()
{
    button_direction_t btn_dir;
    for (;;) {
        if (xQueueReceive(button_intr_queue, &btn_dir, portMAX_DELAY)) {
            // TODO: implement debouncing before calling click handler
            // ESP_LOGI(TAG, "btn intr");
            xQueueSend(input_evt_queue, &btn_dir, NULL);
        }
    }
}


// sets up the pins for the device button inputs
// accepts a callback to be run when a click is detected on any button
void setup_button_input_pins(input_pins_t input_pins) {
    // create the needed queues
    button_intr_queue = xQueueCreate(8, sizeof(button_direction_t));
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

    ESP_ERROR_CHECK(gpio_isr_handler_add(input_pins.up,     button_intr_handler, (void*) BTN_UP));
    ESP_ERROR_CHECK(gpio_isr_handler_add(input_pins.right,  button_intr_handler, (void*) BTN_RIGHT));
    ESP_ERROR_CHECK(gpio_isr_handler_add(input_pins.down,   button_intr_handler, (void*) BTN_DOWN));
    ESP_ERROR_CHECK(gpio_isr_handler_add(input_pins.left,   button_intr_handler, (void*) BTN_LEFT));
    ESP_ERROR_CHECK(gpio_isr_handler_add(input_pins.middle, button_intr_handler, (void*) BTN_MIDDLE));
}