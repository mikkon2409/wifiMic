#include <esp_types.h>
#include <driver/i2s.h>
#include <hal/i2c_types.h>
#include <stdlib.h>
#include <esp_log.h>

#include "wav.h"
#include "sdcard.h"
#include "utils.h"
#include "settings.h"
#include "track_rec.h"


void rec_track(volatile bool* need_to_stop) {
    static const int i2s_num = I2S_NUM_0; // i2s port number
    
    const settings sett = get_settings();
    const int sample_rate = sett.sample_rate;
    const int bits_per_sample = sett.bits_per_sample;
    const int rec_len_sec = sett.rec_lenght_sec;

    const int one_track_byte_len = rec_len_sec * sample_rate * (bits_per_sample / 8) * 2;

    static i2s_config_t i2s_config = {
        .mode = I2S_MODE_MASTER | I2S_MODE_RX | I2S_MODE_PDM,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
        .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
        .communication_format = I2S_COMM_FORMAT_PCM,
        .intr_alloc_flags = 0, // default interrupt priority
        .dma_buf_count = 8,
        .dma_buf_len = 64,
        .use_apll = false};

    i2s_config.sample_rate = sample_rate;
    i2s_config.bits_per_sample = bits_per_sample;

    ESP_ERROR_CHECK(i2s_driver_install(i2s_num, &i2s_config, 0, NULL));
    static const i2s_pin_config_t pin_config = {
        .bck_io_num = I2S_PIN_NO_CHANGE,
        .ws_io_num = GPIO_NUM_25,
        .data_out_num = I2S_PIN_NO_CHANGE,
        .data_in_num = GPIO_NUM_23};

    ESP_ERROR_CHECK(i2s_set_pin(i2s_num, &pin_config));

    wav_header mic1_header;
    init_wav_header(&mic1_header, 2, sample_rate, bits_per_sample);
    file_name mic1_name = get_wav_file_name(mount_point);
    enable_sd_card_vfs_fat();
    FILE* mic1 = fopen(mic1_name.str, "wb");
    write_wav_header(mic1, &mic1_header);

    int8_t buf[128];
    size_t read;
    size_t read_total = 0;
    ESP_LOGI("AUDIO RECORDING", "START");
    while (read_total < one_track_byte_len && !(*need_to_stop)) {
        i2s_read(i2s_num, buf, sizeof(buf), &read, portMAX_DELAY);
        if (read > 0) {
            fwrite(buf, 1, sizeof(buf), mic1);
            fflush(mic1);
            read_total += read;
        }
    }
    ESP_LOGI("AUDIO RECORDING", "END");

    update_header_according_data_size(&mic1_header, (uint32_t)read_total);
    write_wav_header(mic1, &mic1_header);
    fclose(mic1);
    disable_sd_card_vfs_fat();
    ESP_ERROR_CHECK(i2s_driver_uninstall(i2s_num));
}