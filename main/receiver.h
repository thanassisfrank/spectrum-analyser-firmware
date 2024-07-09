// receiver.h
#pragma once

#include "driver/gpio.h"
#include "esp_adc/adc_oneshot.h"

#define IF_FREQ_MHZ 479

#define RECV_SPI_CLK_HZ 100000 // 10KHz

typedef struct {
    gpio_num_t clk, mosi, cs, rssi;
} spi_receiver_pins_t;

typedef struct {
    spi_device_handle_t spi_handle;
    adc_oneshot_unit_handle_t adc_handle;
    adc_channel_t adc_channel;
} receiver_device_t;

void setup_receiver(receiver_device_t*, spi_receiver_pins_t, adc_oneshot_unit_handle_t, adc_channel_t);

void set_receiver_freq_mhz(receiver_device_t*, int);

int get_receiver_rssi(receiver_device_t*);