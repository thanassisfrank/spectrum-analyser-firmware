// gui.c
// functions for drawing the ui based on u8g2

#include "freertos/FreeRTOS.h"
#include "u8g2.h"

#include "gui.h"

#include "bitmaps.h"

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

    // u8g2 logo
    u8g2_ClearBuffer(info->u8g2);
    u8g2_DrawXBM(info->u8g2, 0, 0, u8g2_logo_128x64_width, u8g2_logo_128x64_height, u8g2_logo_128x64_bits);
    vTaskDelay(pdMS_TO_TICKS(info->delay_ms));
    u8g2_SendBuffer(info->u8g2);

    

    // clear screen
    u8g2_ClearBuffer(info->u8g2);
    vTaskDelay(pdMS_TO_TICKS(info->delay_ms));
    u8g2_SendBuffer(info->u8g2);

    // delete self
    vTaskDelete(NULL);
}

void gui_print_string(char* str)
{

}

void gui_draw_main_menu(u8g2_t* u8g2)
{
    
}