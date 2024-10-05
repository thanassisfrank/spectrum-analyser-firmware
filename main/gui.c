// gui.c
// functions for drawing the ui based on u8g2

#include <string.h>
#include "freertos/FreeRTOS.h"
#include "esp_log.h"
#include "u8g2.h"

#include "gui.h"

#include "bitmaps.h"

static const char* TAG = "gui";

// clears the current buffer
// by default, all of the UI functions do not clear the screen so this is need
void gui_clear_screen(u8g2_t* u8g2)
{
    u8g2_ClearBuffer(u8g2);
}

// task to draw the splash images on startup
// delay controls the amount of time each is shown for
void gui_draw_splashes_task(splash_draw_config_t* info)
{
    // espressif logo
    u8g2_ClearBuffer(info->u8g2);
    u8g2_DrawXBM(
        info->u8g2, 
        (128 - esp_logo_50x50_width)/2, 
        (64 - esp_logo_50x50_height)/2, 
        esp_logo_50x50_width,
        esp_logo_50x50_height,
        esp_logo_50x50_bits 
    );
    u8g2_SendBuffer(info->u8g2);

    vTaskDelay(pdMS_TO_TICKS(info->delay_ms));

    // u8g2 logo
    u8g2_ClearBuffer(info->u8g2);
    u8g2_DrawXBM(info->u8g2, 0, 0, u8g2_logo_128x64_width, u8g2_logo_128x64_height, u8g2_logo_128x64_bits);
    u8g2_SendBuffer(info->u8g2);

    vTaskDelay(pdMS_TO_TICKS(info->delay_ms));
    
    // clear screen
    u8g2_ClearBuffer(info->u8g2);
    u8g2_SendBuffer(info->u8g2);

    // delete self
    vTaskDelete(NULL);
}

void gui_draw_splashes_blocking(u8g2_t* u8g2, int delay_ms)
{
    // espressif logo
    u8g2_ClearBuffer(u8g2);
    u8g2_DrawXBM(
        u8g2, 
        (128 - esp_logo_50x50_width)/2, 
        (64 - esp_logo_50x50_height)/2, 
        esp_logo_50x50_width,
        esp_logo_50x50_height,
        esp_logo_50x50_bits 
    );
    u8g2_SendBuffer(u8g2);

    vTaskDelay(pdMS_TO_TICKS(delay_ms));

    // u8g2 logo
    u8g2_ClearBuffer(u8g2);
    u8g2_DrawXBM(u8g2, 0, 0, u8g2_logo_128x64_width, u8g2_logo_128x64_height, u8g2_logo_128x64_bits);
    u8g2_SendBuffer(u8g2);

    vTaskDelay(pdMS_TO_TICKS(delay_ms));
}


void gui_print_string(u8g2_t* u8g2, const char* str)
{
    u8g2_SetFont(u8g2, u8g2_font_ncenB14_tr);
    u8g2_DrawStr(u8g2, 0, 15, str);
    u8g2_SendBuffer(u8g2);
}

void gui_print_lines_select(u8g2_t* u8g2, int x, int y, int select_index, char** lines, int lines_count)
{
    const int line_spacing = 12;
    u8g2_SetFont(u8g2, u8g2_font_spleen6x12_mf);
    char this_line[32] = "";

    for (int i = 0; i < lines_count; i++) {
        // ESP_LOGI(TAG, "printing line %i", i);
        if (i == select_index) {
            strcpy(this_line, ">");
        } else {
            strcpy(this_line, " ");
        }
        strcat(this_line, lines[i]);
        u8g2_DrawStr(u8g2, x, y + (i+1) * line_spacing, (const char*)this_line);
    }
    u8g2_SendBuffer(u8g2);
}


// TODO: support horizontal graph
// draws a bar graph from the supplied data
// x, y give position of top left corner
// col_width is num pixels per column width
// height is total size parallel to columns in pixels
// scale is the data value per pixel parallel to columns
// data is array of ints
// count is number of columns
void gui_draw_bars(u8g2_t* u8g2, int x, int y, int col_width, int height, int scale, int* data, int count)
{
    // clear the area
    u8g2_SetDrawColor(u8g2, 0);
    u8g2_DrawBox(u8g2, x, y, count * col_width, height);

    u8g2_SetDrawColor(u8g2, 1);
    int bar_height = 0;
    for (int i = 0; i < count; i++) {
        bar_height = data[i]/scale;
        u8g2_DrawBox(u8g2, x + col_width * i, height - bar_height, col_width, bar_height);
        // u8g2_DrawBox(u8g2, x + col_width * i, 0, col_width, bar_height);
    }
    // ESP_LOGI(TAG, "%i", bar_height);
    u8g2_SendBuffer(u8g2);
}

// updates a single bar from a bar graph
void gui_update_bar(u8g2_t* u8g2, int x, int y, int col_width, int height, int scale, int data, int index)
{
    
    // clear the area
    u8g2_SetDrawColor(u8g2, 0);
    u8g2_DrawBox(u8g2, x + col_width * index, y, col_width, height);

    // draw bar
    u8g2_SetDrawColor(u8g2, 1);
    int bar_height = data/scale;
    u8g2_DrawBox(u8g2, x + col_width * index, height - bar_height, col_width, bar_height);

    u8g2_SendBuffer(u8g2);
}


void gui_draw_main_menu(u8g2_t* u8g2)
{
    
}

