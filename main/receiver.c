// receiver.c
// handles interaction with the radio receiver module

#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "driver/spi_master.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"

#include "receiver.h"

// receiver device
static receiver_device_t receiver_device;

// receiver command queue
static QueueHandle_t receiver_command_queue = NULL;
// receiver command consumption task
static TaskHandle_t receiver_command_task_handle = NULL;
// output rssi reading queue
static QueueHandle_t rssi_reading_queue = NULL;

// external access to the rssi reading queue
BaseType_t receive_rssi_queue(rssi_reading_t* reading, TickType_t ticks_to_wait)
{
    return xQueueReceive(rssi_reading_queue, reading, ticks_to_wait);
}


// task for consuming receiver command queue
static void receiver_command_task()
{
    receiver_command_t command;
    rssi_reading_t rssi_reading;
    for (;;) {
        if (xQueueReceive(receiver_command_queue, &command, portMAX_DELAY)) {
            // check if the requested freq matches current
            // switch to correct freq and delay if not
            if (command.freq != receiver_device.curr_freq) {
                int freq_lo_mhz = command.freq - IF_FREQ_MHZ;
                int N = freq_lo_mhz/(2*32); // 84
                int A = freq_lo_mhz/2 - 32*N; // 5
                uint32_t data = A | (N << 7);


                spi_transaction_t trans_desc;
                trans_desc.addr = 0b1000; // address 1 but bits flipped
                trans_desc.cmd = 1; // 1 for write
                trans_desc.flags = 0;
                trans_desc.length = 20; // number of data bits
                trans_desc.rxlength = 0;
                trans_desc.tx_buffer = &data;
                trans_desc.rx_buffer = NULL;

                ESP_ERROR_CHECK(spi_device_transmit(receiver_device.spi_handle, &trans_desc));

                // delay to allow settling
                vTaskDelay(pdMS_TO_TICKS(RECV_SETTLE_MS));

                receiver_device.curr_freq = command.freq;
            }

            // read rssi and write to output queue
            // Vout = Dout * Vmax / Dmax
            // dmax = 2^bitwidth
            adc_oneshot_read(receiver_device.adc_handle, receiver_device.adc_channel, &rssi_reading.rssi);
            rssi_reading.freq = receiver_device.curr_freq;
            xQueueSend(rssi_reading_queue, &rssi_reading, 0);
        }
    }
}


// set up the receiver on SPI2_HOST
void setup_receiver(spi_receiver_pins_t receiver_pins, adc_oneshot_unit_handle_t adc_handle, adc_channel_t rssi_channel)
{
    // spi bus initialisation is done before this is called
    spi_device_interface_config_t dev_config;
    dev_config.address_bits = 4;
    dev_config.command_bits = 1;
    dev_config.dummy_bits = 0;
    dev_config.mode = 0;
    dev_config.duty_cycle_pos = 0;
    dev_config.cs_ena_posttrans = 0;
    dev_config.cs_ena_pretrans = 0;
    dev_config.clock_source = SPI_CLK_SRC_DEFAULT;
    dev_config.clock_speed_hz = RECV_SPI_CLK_HZ;
    dev_config.spics_io_num = receiver_pins.cs;
    dev_config.flags = SPI_DEVICE_TXBIT_LSBFIRST;
    dev_config.queue_size = 200;
    dev_config.pre_cb = NULL;
    dev_config.post_cb = NULL;
    // ESP_LOGI(TAG, "... Adding device bus.");
    ESP_ERROR_CHECK(spi_bus_add_device(SPI2_HOST, &dev_config, &(receiver_device.spi_handle)));

    receiver_device.adc_handle = adc_handle;
    receiver_device.adc_channel = rssi_channel;

    // setup the queues
    receiver_command_queue = xQueueCreate(64, sizeof(receiver_command_t));
    rssi_reading_queue = xQueueCreate(64, sizeof(rssi_reading_t));
    // start the task to consume button click events
    xTaskCreate(receiver_command_task, "receiver_command_task", 2048, NULL, 10, &receiver_command_task_handle);

}

// enqueues a read at the given frequency for the receiver
void request_receiver_rssi(int freq_mhz)
{

    receiver_command_t command = {
        .freq = freq_mhz,
    };

    xQueueSend(receiver_command_queue, &command, pdMS_TO_TICKS(5));
}

// dispatches a group of rssi requests to the receiver command queue
void request_receiver_sweep(int start_freq, int step, int count) {
    for (int i = 0; i < count; i++) {
        request_receiver_rssi(start_freq + i * step);
    }
}

