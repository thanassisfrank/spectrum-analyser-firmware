// ws2812.c
// driver for ws2812 rgb led
// crated with reference to https://github.com/espressif/esp-idf/tree/v5.2.2/examples/peripherals/rmt/led_strip/main
#include "esp_attr.h"
#include "esp_log.h"
#include "driver/rmt_tx.h"

#include "ws2812.h"

static const char* TAG = "ws2812";


// member functions of the base rmt_encoder_t ===============================================
// this function needs to handle being interrupted and thus keeps track of the state
static size_t IRAM_ATTR rmt_encode_ws2812 (
    rmt_encoder_t* encoder, 
    rmt_channel_handle_t channel, 
    const void *primary_data, 
    size_t data_size, 
    rmt_encode_state_t *ret_state
)
{
    rmt_ws2812_encoder_t* ws2812_encoder = __containerof(encoder, rmt_ws2812_encoder_t, base);
    rmt_encode_state_t session_state = RMT_ENCODING_RESET;
    rmt_encode_state_t state = RMT_ENCODING_RESET;
    size_t encoded_symbols = 0;
    switch (ws2812_encoder->state) {
        case 0: // send RGB data
            // use the byte encoder
            encoded_symbols += ws2812_encoder->bytes_encoder->encode(
                ws2812_encoder->bytes_encoder, 
                channel, 
                primary_data, 
                data_size, 
                &session_state
            );
            if (session_state & RMT_ENCODING_COMPLETE) {
                ws2812_encoder->state = 1; // switch to next state when current encoding session finished
            }
            if (session_state & RMT_ENCODING_MEM_FULL) {
                state |= RMT_ENCODING_MEM_FULL;
                goto out; // yield if there's no free space for encoding artifacts
            }
        // fall-through
        case 1: // send reset code
            encoded_symbols += ws2812_encoder->copy_encoder->encode(
                ws2812_encoder->copy_encoder, 
                channel, 
                &ws2812_encoder->reset_code,
                sizeof(ws2812_encoder->reset_code), 
                &session_state
            );
            if (session_state & RMT_ENCODING_COMPLETE) {
                ws2812_encoder->state = RMT_ENCODING_RESET; // back to the initial encoding session
                state |= RMT_ENCODING_COMPLETE;
            }
            if (session_state & RMT_ENCODING_MEM_FULL) {
                state |= RMT_ENCODING_MEM_FULL;
                goto out; // yield if there's no free space for encoding artifacts
            }
    }
out:
    *ret_state = state;
    return encoded_symbols;
}

// need to provide a function to delete encode
static esp_err_t rmt_del_ws2812_encoder(rmt_encoder_t *encoder)
{
    rmt_ws2812_encoder_t *ws2812_encoder = __containerof(encoder, rmt_ws2812_encoder_t, base);
    rmt_del_encoder(ws2812_encoder->bytes_encoder);
    rmt_del_encoder(ws2812_encoder->copy_encoder);
    free(ws2812_encoder);
    return ESP_OK;
}

// function to reset encoder
static esp_err_t rmt_ws2812_encoder_reset(rmt_encoder_t *encoder)
{
    rmt_ws2812_encoder_t *ws2812_encoder = __containerof(encoder, rmt_ws2812_encoder_t, base);
    rmt_encoder_reset(ws2812_encoder->bytes_encoder);
    rmt_encoder_reset(ws2812_encoder->copy_encoder);
    ws2812_encoder->state = RMT_ENCODING_RESET;
    return ESP_OK;
}

// ==========================================================================================


// returns a new rgb led instance on the supplied pin
ws2812_t create_ws2812(gpio_num_t pin) 
{
    // create a new rmt tx channel
    const rmt_tx_channel_config_t config = {
        .gpio_num = pin,
        .clk_src = RMT_CLK_SRC_DEFAULT,
        .resolution_hz = 10 * 1000 * 1000, // 10MHz
        .mem_block_symbols = 64,
        .trans_queue_depth = 4
    };
    rmt_channel_handle_t tx_channel = NULL;
    ESP_ERROR_CHECK(rmt_new_tx_channel(&config, &tx_channel));

    // create encoder
    rmt_ws2812_encoder_t *ws2812_encoder = NULL;
    ws2812_encoder = rmt_alloc_encoder_mem(sizeof(rmt_ws2812_encoder_t));
    ws2812_encoder->base.encode = rmt_encode_ws2812;
    ws2812_encoder->base.del = rmt_del_ws2812_encoder;
    ws2812_encoder->base.reset = rmt_ws2812_encoder_reset;
    // different led strip might have its own timing requirements, following parameter is for WS2812
    rmt_bytes_encoder_config_t bytes_encoder_config = {
        .bit0 = {
            .level0 = 1,
            .duration0 = 0.4 * config.resolution_hz / 1000000, // T0H=0.3us
            .level1 = 0,
            .duration1 = 0.8 * config.resolution_hz / 1000000, // T0L=0.9us
        },
        .bit1 = {
            .level0 = 1,
            .duration0 = 0.8 * config.resolution_hz / 1000000, // T1H=0.9us
            .level1 = 0,
            .duration1 = 0.4 * config.resolution_hz / 1000000, // T1L=0.3us
        },
        .flags.msb_first = 1 // WS2812 transfer bit order: G7...G0R7...R0B7...B0
    };
    ESP_ERROR_CHECK(rmt_new_bytes_encoder(&bytes_encoder_config, &ws2812_encoder->bytes_encoder));
    rmt_copy_encoder_config_t copy_encoder_config = {};
    ESP_ERROR_CHECK(rmt_new_copy_encoder(&copy_encoder_config, &ws2812_encoder->copy_encoder));

    uint32_t reset_ticks = config.resolution_hz / 1000000 * 500; // reset code duration defaults to 500us
    ws2812_encoder->reset_code = (rmt_symbol_word_t) {
        .level0 = 0,
        .duration0 = reset_ticks / 2,
        .level1 = 0,
        .duration1 = reset_ticks / 2,
    };
    
    // enable the channel
    rmt_enable(tx_channel);

    return (ws2812_t){
        .pin = pin,
        .tx_channel = tx_channel,
        .encoder = &ws2812_encoder->base,
    };
}

// sets the colour of the supplied led
void set_color_ws2812(ws2812_t* led, rgbuint8_t col) 
{
    ESP_LOGI(TAG, "displaying r: %u, g: %u, b: %u", col.r, col.g, col.b);
    rmt_transmit_config_t config = {
        .loop_count = 0,
    };
    ESP_ERROR_CHECK(rmt_transmit(led->tx_channel, led->encoder, col.buff, 3, &config));
}