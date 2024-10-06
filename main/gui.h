// gui.h
#pragma once

#include "u8g2.h"
#include "gui.h"

typedef struct {
    const uint8_t* chars;
    int height, width;
    // these are y values of these lines from the top of the character
    int capline, median, baseline;

} font_info_t;

static const font_info_t FONT_SMALL = {u8g2_font_spleen6x12_mf, 12, 6, 0, 2, 9};
static const font_info_t FONT_LARGE = {u8g2_font_spleen12x24_mf, 24, 12, 3, 7, 19};

typedef enum {
    CHAR_SIZE_CAPITAL,
    CHAR_SIZE_SMALL,
    CHAR_SIZE_DESCENDER,
    CHAR_SIZE_FULL
} char_size_t;


typedef struct {
    u8g2_t* u8g2;
    int delay_ms;
} splash_draw_config_t;

void gui_draw_splashes_task(splash_draw_config_t*);

void gui_draw_splashes_blocking(u8g2_t* u8g2, int delay_ms);

void gui_clear_screen(u8g2_t* u8g2);

void gui_draw_string(u8g2_t* u8g2, int x, int y, font_info_t* font, const char* str);

void gui_draw_string_vertical_arrows(u8g2_t* u8g2, int x, int y, font_info_t* font, const char* str, char_size_t char_size);

void gui_draw_select_lines(u8g2_t* u8g2, int x, int y, font_info_t* font, int select_index, const char** lines, int lines_count);

void gui_draw_bars(u8g2_t* u8g2, int x, int y, int col_width, int height, int scale, int* data, int count);

void gui_update_bar(u8g2_t* u8g2, int x, int y, int col_width, int height, int scale, int data, int index);
