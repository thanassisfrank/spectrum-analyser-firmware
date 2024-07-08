// receiver.c
// handles interaction with the radio receiver module

#include "esp_err.h"
#include "driver/spi_master.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"

#include "receiver.h"

// set up the receiver on SPI2_HOST
void setup_receiver(receiver_device_t* receiver, spi_receiver_pins_t receiver_pins, adc_oneshot_unit_handle_t adc_handle, adc_channel_t rssi_channel)
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
    dev_config.flags = 0;
    dev_config.queue_size = 200;
    dev_config.pre_cb = NULL;
    dev_config.post_cb = NULL;
    // ESP_LOGI(TAG, "... Adding device bus.");
    ESP_ERROR_CHECK(spi_bus_add_device(SPI2_HOST, &dev_config, &(receiver->spi_handle)));

    receiver->adc_handle = adc_handle;
    receiver->adc_channel = rssi_channel;

}

// set the frequency of the receiver in MHz
void set_receiver_freq_mhz(receiver_device_t* receiver, int freq_mhz)
{
    int freq_lo_mhz = freq_mhz - IF_FREQ_MHZ;
    int N = freq_lo_mhz/(2*32); // 84
    int A = freq_lo_mhz/2 - 32*N; // 5
    uint32_t data = A | (N << 7);


    spi_transaction_t trans_desc;
    trans_desc.addr = 0x01;
    trans_desc.cmd = 1; // 1 for write
    trans_desc.flags = 0;
    trans_desc.length = 20; // number of data bits
    trans_desc.rxlength = 0;
    trans_desc.tx_buffer = &data;
    trans_desc.rx_buffer = NULL;

    ESP_ERROR_CHECK(spi_device_transmit(receiver->spi_handle, &trans_desc));
}

// reads the current rssi value as an integer
int get_receiver_rssi(receiver_device_t* receiver)
{
    // Vout = Dout * Vmax / Dmax
    // dmax = 2^bitwidth
    int rssi_raw;
    adc_oneshot_read(receiver->adc_handle, receiver->adc_channel, &rssi_raw);
    return rssi_raw;
}
