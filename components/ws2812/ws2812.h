// ws2812.h
#include <stdint.h>
#include "driver/gpio.h"
#include "driver/rmt_tx.h"

typedef struct {
    rmt_encoder_t base;
    rmt_encoder_t *bytes_encoder;
    rmt_encoder_t *copy_encoder;
    int state;
    rmt_symbol_word_t reset_code;
} rmt_ws2812_encoder_t;

typedef struct {
    gpio_num_t pin;
    rmt_channel_handle_t tx_channel;
    rmt_encoder_t* encoder;

} ws2812_t;

typedef union {
    uint8_t buff[3];
    struct {
        uint8_t r;
        uint8_t g; 
        uint8_t b;
    };
} rgbuint8_t;

// some example colors
#define WS2812_RED (rgbuint8_t){.r = 255, .g = 0, .b = 0}
#define WS2812_GREEN (rgbuint8_t){.r = 0, .g = 255, .b = 0}
#define WS2812_BLUE (rgbuint8_t){.r = 0, .g = 0, .b = 255}

#define WS2812_DIM_RED (rgbuint8_t){.r = 10, .g = 0, .b = 0}
#define WS2812_DIM_GREEN (rgbuint8_t){.r = 0, .g = 10, .b = 0}
#define WS2812_DIM_BLUE (rgbuint8_t){.r = 0, .g = 0, .b = 10}

#define WS2812_PINK (rgbuint8_t){.r = 255, .g = 0, .b = 255}
#define WS2812_YELLOW (rgbuint8_t){.r = 255, .g = 255, .b = 0}
#define WS2812_CYAN (rgbuint8_t){.r = 0, .g = 255, .b = 255}

#define WS2812_DIM_PINK (rgbuint8_t){.r = 5, .g = 0, .b = 5}
#define WS2812_DIM_YELLOW (rgbuint8_t){.r = 5, .g = 5, .b = 0}
#define WS2812_DIM_CYAN (rgbuint8_t){.r = 0, .g = 5, .b = 5}

#define WS2812_WHITE (rgbuint8_t){.r = 255, .g = 255, .b = 255}
#define WS2812_BLACK (rgbuint8_t){.r = 0, .g = 0, .b = 0}

ws2812_t create_ws2812( gpio_num_t pin);

void set_color_ws2812(ws2812_t* led, rgbuint8_t col);