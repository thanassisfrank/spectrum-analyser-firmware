// receiver.h
#pragma once

#include "driver/gpio.h"
#include "esp_adc/adc_oneshot.h"

#define IF_FREQ_MHZ 479

#define RECV_SPI_CLK_HZ 1000000 // 1MHz

#define RECV_SETTLE_MS 50

// from RTC6715 datasheet:
// rssi voltage 0.5~1.1V
#define RECV_0_RSSI_MILLIVOLTS 400
#define RECV_1000_RSSI_MILLIVOLTS 1100

typedef struct {
    gpio_num_t clk, mosi, cs, rssi;
} spi_receiver_pins_t;

typedef struct {
    spi_device_handle_t spi_handle;
    adc_oneshot_unit_handle_t adc_handle;
    adc_channel_t adc_channel;
    adc_cali_handle_t adc_cali_handle;
    int curr_freq;
} receiver_device_t;

typedef enum {
    NONE,
    READ_RSSI,
    SET_FREQ
} receiver_command_type_t;

typedef struct {
    int freq;
} receiver_command_t;

typedef struct {
    int freq, rssi;
} rssi_reading_t;

BaseType_t receive_rssi_queue(rssi_reading_t*, TickType_t);

void setup_receiver(spi_receiver_pins_t, adc_oneshot_unit_handle_t, adc_channel_t, adc_cali_handle_t);

void request_receiver_rssi(int);

void request_receiver_sweep(int, int, int);
