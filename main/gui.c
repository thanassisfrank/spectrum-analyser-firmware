// gui.c
// functions for drawing the ui based on u8g2

#include "u8g2.h"

#include "gui.h"

#include "bitmaps.h"

void gui_draw_splash(u8g2_t* u8g2)
{
    u8g2_ClearBuffer(u8g2);
    u8g2_DrawXBM(u8g2, 0, 0, u8g2_logo_128x64_width, u8g2_logo_128x64_height, u8g2_logo_128x64_bits);
    u8g2_SendBuffer(u8g2);
}

void gui_draw_main_menu(u8g2_t* u8g2)
{
    
}