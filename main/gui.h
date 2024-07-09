// gui.h
#pragma once

#include "u8g2.h"
#include "gui.h"

typedef struct {
    u8g2_t* u8g2;
    int delay_ms;
} splash_draw_config_t;

void gui_draw_splashes_task(splash_draw_config_t*);

void gui_draw_main_menu(u8g2_t*);