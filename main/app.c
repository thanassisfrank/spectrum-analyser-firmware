// app.c
#include <string.h>
#include "esp_log.h"

#include "app.h"
#include "gui.h"
#include "receiver.h"

static const char* TAG = "app";

// changes the current screen to the requested one
void app_switch_screens(app_state_t* app_state, app_screen_t new_screen)
{
    // clear the buffer
    gui_clear_screen(&app_state->u8g2);
    
    // reset selected index
    app_state->selected_index = 0;

    switch (new_screen) 
    {
        case SCREEN_LOADING:
            // display the splash screen
            gui_draw_splashes_blocking(&app_state->u8g2, 1000);
            break;
        case SCREEN_MAIN_MENU:
            gui_draw_select_lines(&app_state->u8g2, 0, 0, &FONT_SMALL, app_state->selected_index, main_menu_lines, 5);
            break;
        case SCREEN_CHANNEL_MONITOR:
            freq_band_t curr_band = freq_bands[app_state->selected_index];
            gui_draw_string_vertical_arrows(&app_state->u8g2, 8, 18, &FONT_LARGE, FREQ_BAND_NAME[curr_band], CHAR_SIZE_CAPITAL);
            
            size_t channel_count = sizeof(freq_channels)/sizeof(freq_channels[0]);
            char channel_names_str[16] = "";
            for (int i = 0; i < channel_count; i++) {
                strcat(channel_names_str, FREQ_CHANNEL_NAME[freq_channels[i]]); 
            }
            gui_draw_string(&app_state->u8g2, 40, 55, &FONT_SMALL, channel_names_str);
            break;
        default:
            break;
    }

    app_state->current_screen = new_screen;
}


// click handler
void app_click_handler(app_state_t* app_state, button_direction_t btn_dir)
{
    switch (app_state->current_screen) 
    {
        case SCREEN_MAIN_MENU:
            main_menu_click_handler(app_state, btn_dir);
            break;
        case SCREEN_SPECTRUM_SWEEP:
            spectrum_sweep_click_handler(app_state, btn_dir);
            break;
        case SCREEN_CHANNEL_MONITOR:
            channel_monitor_click_handler(app_state, btn_dir);
            break;
        default:
            ESP_LOGI(TAG, "btn");
            break;
    }
}


// rssi reading handler
void app_rssi_reading_handler(app_state_t* app_state, rssi_reading_t rssi_reading)
{
    switch (app_state->current_screen) 
    {
        case SCREEN_SPECTRUM_SWEEP:
            // write the reading into the correct place in readings
            int index = (rssi_reading.freq - 5650)/5;

            gui_update_bar(&app_state->u8g2, 0, 0, 2, 64, 20, rssi_reading.rssi, index);

            // ESP_LOGI(TAG, "recv %i", rssi_reading.rssi);
            break;
        case SCREEN_CHANNEL_MONITOR:
            channel_monitor_rssi_handler(app_state, rssi_reading);
            break;
        default:
            ESP_LOGI(TAG, "rssi");
            break;
    }
}


// screen specific functions ===========================================================================

void main_menu_click_handler(app_state_t* app_state, button_direction_t btn_dir)
{
    size_t main_menu_entries = 5;
    switch (btn_dir)
    {
        case BTN_UP:
            // move cursor up
            if (0 == app_state->selected_index) {
                app_state->selected_index = main_menu_entries - 1;
            } else {
                app_state->selected_index--;
            }
            gui_draw_select_lines(&app_state->u8g2, 0, 0, &FONT_SMALL, app_state->selected_index, main_menu_lines, main_menu_entries);
            break;
        case BTN_DOWN:
            // move cursor down
            app_state->selected_index = (app_state->selected_index + 1) % main_menu_entries;
            gui_draw_select_lines(&app_state->u8g2, 0, 0, &FONT_SMALL, app_state->selected_index, main_menu_lines, main_menu_entries);
            break;
        case BTN_RIGHT:
            // open the right app
            if (0 == app_state->selected_index) {
                app_switch_screens(app_state, SCREEN_SPECTRUM_SWEEP);
            } else if (1 == app_state->selected_index) {
                app_switch_screens(app_state, SCREEN_CHANNEL_MONITOR);
            }
            break;
        default:
            break;
    }
}


void spectrum_sweep_click_handler(app_state_t* app_state, button_direction_t btn_dir)
{
    // request spectrum sweep
    if (BTN_LEFT == btn_dir) {
        app_switch_screens(app_state, SCREEN_MAIN_MENU);
    } else {
        request_receiver_sweep(5650, 5, 64);
    }
}


void channel_monitor_click_handler(app_state_t* app_state, button_direction_t btn_dir)
{
    size_t band_count = sizeof(freq_bands)/sizeof(freq_bands[0]);
    size_t channel_count = sizeof(freq_channels)/sizeof(freq_channels[0]);
    freq_band_t curr_band;

    switch (btn_dir)
    {
        case BTN_UP:
            if (0 == app_state->selected_index) {
                app_state->selected_index = band_count - 1;
            } else {
                app_state->selected_index--;
            }
            curr_band = freq_bands[app_state->selected_index];
            gui_draw_string_vertical_arrows(&app_state->u8g2, 8, 18, &FONT_LARGE, FREQ_BAND_NAME[curr_band], CHAR_SIZE_CAPITAL);
            break;
        case BTN_DOWN:
            app_state->selected_index = (app_state->selected_index + 1) % band_count;
            curr_band = freq_bands[app_state->selected_index];
            gui_draw_string_vertical_arrows(&app_state->u8g2, 8, 18, &FONT_LARGE, FREQ_BAND_NAME[curr_band], CHAR_SIZE_CAPITAL);
            break;
        case BTN_LEFT:
            app_switch_screens(app_state, SCREEN_MAIN_MENU);
            break;
        case BTN_RIGHT:
            // do a scan
            curr_band = freq_bands[app_state->selected_index];
            for (int i = 0; i < channel_count; i++) {
                request_receiver_rssi(FREQ_MAP[curr_band][freq_channels[i]]);
            }
            break;
        default:
            break;
    }
}

void channel_monitor_rssi_handler(app_state_t* app_state, rssi_reading_t rssi_reading)
{
    freq_band_t band = freq_bands[app_state->selected_index];
    // loop through all channels in the current band and check if frequency matches
    size_t channel_count = sizeof(freq_channels)/sizeof(freq_channels[0]);

    for (int i = 0; i < channel_count; i++) {
        freq_channel_t channel = freq_channels[i];
        if (FREQ_MAP[band][channel] == rssi_reading.freq) {
            // matches one of the channels in the current band
            // update the display
            gui_update_bar(&app_state->u8g2, 40, 5, 6, 50, 20, rssi_reading.rssi, i);
            break;
        }
    }
}