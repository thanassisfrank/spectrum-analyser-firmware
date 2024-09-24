// receiver.h
#pragma once

#include "driver/gpio.h"
#include "esp_adc/adc_oneshot.h"

#define IF_FREQ_MHZ 479

#define RECV_SPI_CLK_HZ 100000 // 10KHz

#define RECV_SETTLE_MS 10

typedef struct {
    gpio_num_t clk, mosi, cs, rssi;
} spi_receiver_pins_t;

typedef struct {
    spi_device_handle_t spi_handle;
    adc_oneshot_unit_handle_t adc_handle;
    adc_channel_t adc_channel;
} receiver_device_t;

typedef enum {
    NONE,
    READ_RSSI,
    SET_FREQ
} receiver_command_type_t;

typedef struct {
    receiver_command_type_t command_type;
    int data;
} receiver_command_t;

typedef struct {
    int freq, rssi;
} rssi_reading_t;

BaseType_t receive_rssi_queue(rssi_reading_t*, TickType_t);

void setup_receiver(spi_receiver_pins_t, adc_oneshot_unit_handle_t, adc_channel_t);

void request_receiver_freq_mhz(int);

void request_receiver_rssi();