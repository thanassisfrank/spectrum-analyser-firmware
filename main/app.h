// app.h
// contains definitions for overall app flow

#pragma once

#include "freertos/FreeRTOS.h"
#include "lua.h"
#include "u8g2.h"

#include "input.h"
#include "receiver.h"

typedef enum {
    SCREEN_NONE,
    SCREEN_LOADING,
    SCREEN_MAIN_MENU,
    SCREEN_SPECTRUM_SWEEP,
    SCREEN_CHANNEL_MONITOR,
    SCREEN_DRONE_FINDER,
    SCREEN_SETTINGS
} app_screen_t;

static const char* main_menu_lines[5] = {
    "Spectrum sweep",
    "Channel monitor",
    "Drone finder",
    "Lap timer",
    "Settings",
};

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

void app_switch_screens(app_state_t* app_state, app_screen_t new_screen);

void app_click_handler(app_state_t* app_state, button_direction_t btn_dir);

void app_rssi_reading_handler(app_state_t* app_state, rssi_reading_t rssi_reading);

// screen specific functions

void main_menu_click_handler(app_state_t* app_state, button_direction_t btn_dir);

void spectrum_sweep_click_handler(app_state_t* app_state, button_direction_t btn_dir);


void channel_monitor_click_handler(app_state_t* app_state, button_direction_t btn_dir);

void channel_monitor_rssi_handler(app_state_t* app_state, rssi_reading_t rssi_reading);