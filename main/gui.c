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


void gui_draw_string(u8g2_t* u8g2, int x, int y, font_info_t* font, const char* str)
{
    u8g2_SetFont(u8g2, font->chars);
    u8g2_DrawStr(u8g2, x, y + font->baseline, str);
    u8g2_SendBuffer(u8g2);
}

void gui_draw_select_lines(u8g2_t* u8g2, int x, int y, font_info_t* font, int select_index, const char** lines, int lines_count)
{
    u8g2_SetFont(u8g2, font->chars);
    char this_line[32] = "";

    for (int i = 0; i < lines_count; i++) {
        // ESP_LOGI(TAG, "printing line %i", i);
        if (i == select_index) {
            strcpy(this_line, ">");
        } else {
            strcpy(this_line, " ");
        }
        strcat(this_line, lines[i]);
        int offset = font->baseline + i * font->height;
        u8g2_DrawStr(u8g2, x, y + offset, (const char*)this_line);
    }
    u8g2_SendBuffer(u8g2);
}

void gui_draw_string_vertical_arrows(u8g2_t* u8g2, int x, int y, font_info_t* font, const char* str, char_size_t char_size)
{
    // draw text
    u8g2_SetFont(u8g2, font->chars);
    u8g2_DrawStr(u8g2, x, y + font->baseline, str);
    
    size_t char_count = strlen(str);
    
    const int arrow_offset = 4;
    
    int tri_offset_x = (char_count * font->width - 6)/2;
    int upper_offset_y, lower_offset_y;

    switch (char_size)
    {
        case CHAR_SIZE_CAPITAL:
            upper_offset_y = font->capline;
            lower_offset_y = font->baseline;
            break;
        case CHAR_SIZE_SMALL:
            upper_offset_y = font->median;
            lower_offset_y = font->baseline;
            break;
        case CHAR_SIZE_DESCENDER:
            upper_offset_y = font->median;
            lower_offset_y = font->height;
            break;
        case CHAR_SIZE_FULL:
        default:
            upper_offset_y = -1;
            lower_offset_y = font->height;
            break;

    }
    u8g2_DrawXBM(
        u8g2, 
        x + tri_offset_x, 
        y + upper_offset_y - arrow_up_6x3_height + 1 - arrow_offset, 
        arrow_up_6x3_width, 
        arrow_up_6x3_height, 
        arrow_up_6x3_bits
    );
    u8g2_DrawXBM(
        u8g2, 
        x + tri_offset_x, 
        y + lower_offset_y + arrow_offset, 
        arrow_down_6x3_width, 
        arrow_down_6x3_height, 
        arrow_down_6x3_bits
    );
    
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
