// app.h
// contains definitions for overall app flow

#pragma once

#include "u8g2.h"

#include "input.h"

typedef enum {
    SCREEN_NONE,
    SCREEN_LOADING,
    SCREEN_MAIN_MENU,
    SCREEN_SPECTRUM_SWEEP,
    SCREEN_SETTINGS
} app_screen_t;

typedef struct {
    // enum for the current screen
    app_screen_t current_screen;
    // context for the display
    u8g2_t u8g2;
    // handle for lua environment
    lua_State* L;
    // name of the current script
    char* current_script;
    // index of currently selected element
    int selected_index;
} app_state_t;