// input.h

#pragma once

#include "driver/gpio.h"

typedef enum {
    BTN_LEFT,
    BTN_UP,
    BTN_RIGHT,
    BTN_DOWN,
    BTN_MIDDLE,
} button_direction_t;

typedef struct {
    gpio_num_t left, up, right, down, middle;
} input_pins_t;

typedef void (*input_click_handler_t)(button_direction_t);

void setup_button_input_pins(input_pins_t, input_click_handler_t);
