// input.h

#pragma once

#include "driver/gpio.h"

#define BTN_DEBOUNCE_MS 75

typedef enum {
    BTN_LEFT,
    BTN_UP,
    BTN_RIGHT,
    BTN_DOWN,
    BTN_MIDDLE,
} button_direction_t;

typedef enum {
    BTN_EVENT_DOWN,
    BTN_EVENT_UP,
    BTN_EVENT_HOLD
} button_event_t;

typedef struct {
    button_direction_t btn_direction;
    gpio_num_t pin;
} button_t;

typedef struct {
    button_direction_t btn_direction;
    button_event_t event;
} input_event_t;

typedef struct {
    gpio_num_t left, up, right, down, middle;
} input_pins_t;

BaseType_t receive_input_event_queue(button_direction_t*, TickType_t);

void setup_button_input_pins(input_pins_t);
