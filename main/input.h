// input.h

#pragma once

typedef enum {
    BUTTON_DOWN,
    BUTTON_UP,
} button_state_t;

typedef struct {
    button_state_t left, up, right, down, middle;
} input_state_t;