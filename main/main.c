#include "stdio.h"
#include "string.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "sdmmc_cmd.h"
#include "driver/sdmmc_host.h"
#include "esp_vfs_fat.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "driver/i2s.h"
#include "hal/i2c_types.h"
// #include "esp_spiffs.h"

#include "rgb_led.h"
#include "input_gpio.h"
#include "wav.h"
#include "ftp_client.h"
#include "wifi.h"

// const static gpio_num_t RED_LED_PIN = GPIO_NUM_26;
// const static gpio_num_t GREEN_LED_PIN = GPIO_NUM_19;
// const static gpio_num_t BLUE_LED_PIN = GPIO_NUM_27;
// const static gpio_num_t POWER_CONNECTION_PIN = GPIO_NUM_22;
// const static gpio_num_t BUTTON_CONNECTION_PIN = GPIO_NUM_5;
static gpio_num_t SD_CARD_CONN_DET = GPIO_NUM_9;

void app_main(void)
{
    ESP_LOGI("MAIN", "START");

    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    startSTA();

    // start_webserver();

    // const int delay = 500 / portTICK_PERIOD_MS;
    // rgb_gpio rgb_led = {
    //     .red = RED_LED_PIN,
    //     .green = GREEN_LED_PIN,
    //     .blue = BLUE_LED_PIN
    // };
    // init_led_gpio(rgb_led);
    // // for(;;) {
    //     for (int color = 0; color < 3; color++) {
    //         en_led(((int*)(&rgb_led))[color]);
    //         ESP_LOGI("LED", "ENABLED");
    //         vTaskDelay(delay);
    //         dis_led(((int*)(&rgb_led))[color]);
    //         ESP_LOGI("LED", "DISABLED");
    //         vTaskDelay(delay);
    //     }
    // }

    // input_config_t power = {
    //     .connection = POWER_CONNECTION_PIN,
    //     .enabled_state = 1
    // };

    // input_config_t button = {
    //     .connection = BUTTON_CONNECTION_PIN,
    //     .enabled_state = 0
    // };

    // input_floating_init(power);
    // input_floating_init(button);
    // while(1) {
    //     if (is_input_enabled(power)) {
    //         en_led(rgb_led.red);
    //     } else {
    //         dis_led(rgb_led.red);
    //     }
    //     if (is_input_enabled(button)) {
    //         en_led(rgb_led.blue);
    //     } else {
    //         dis_led(rgb_led.blue);
    //     }
    //     vTaskDelay(10 / portTICK_PERIOD_MS);
    // }


    gpio_reset_pin(GPIO_NUM_18);
    gpio_set_direction(GPIO_NUM_18, GPIO_MODE_OUTPUT_OD);
    gpio_set_level(GPIO_NUM_18, 0);
    sdmmc_host_t host = SDMMC_HOST_DEFAULT();
    sdmmc_slot_config_t slot = SDMMC_SLOT_CONFIG_DEFAULT();
    slot.gpio_cd = SD_CARD_CONN_DET;
    esp_vfs_fat_mount_config_t mount_config = {
        .format_if_mount_failed = 1,
        .max_files = 2,
        .allocation_unit_size = 128 * 512
    };
    sdmmc_card_t* card_info = NULL;
    #define MOUNT_POINT "/sdcard"
    ESP_ERROR_CHECK(esp_vfs_fat_sdmmc_mount(MOUNT_POINT, &host, &slot, &mount_config, &card_info));
    sdmmc_card_print_info(stdout, card_info);
    

    // static const int i2s_num = I2S_NUM_0; // i2s port number

    // static  i2s_config_t i2s_config = {
    //     .mode = I2S_MODE_MASTER | I2S_MODE_RX | I2S_MODE_PDM,
    //     .sample_rate = 44100,
    //     .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
    //     .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
    //     .communication_format = I2S_COMM_FORMAT_PCM,
    //     .intr_alloc_flags = 0, // default interrupt priority
    //     .dma_buf_count = 8,
    //     .dma_buf_len = 64,
    //     .use_apll = false};

    // ESP_ERROR_CHECK(i2s_driver_install(i2s_num, &i2s_config, 0, NULL));
    // static const i2s_pin_config_t pin_config = {
    //     .bck_io_num = I2S_PIN_NO_CHANGE,
    //     .ws_io_num = GPIO_NUM_25,
    //     .data_out_num = I2S_PIN_NO_CHANGE,
    //     .data_in_num = GPIO_NUM_23};

    // ESP_ERROR_CHECK(i2s_set_pin(i2s_num, &pin_config));

    // wav_header header;
    // init_wav_header(&header, 2, 44100, 16);
    // FILE* wav = fopen(MOUNT_POINT"/test.wav", "wb");
    // write_wav_header(wav, &header);

    // int16_t buf[64];
    // size_t read;
    // uint32_t read_total = 0;
    // ESP_LOGI("AUDIO RECORDING", "START");
    // for (size_t i = 0; i < 10000; ++i) {
    //     i2s_read(i2s_num, (char*)buf, sizeof(buf), &read, portMAX_DELAY);
    //     if (read > 0)
    //         fwrite(buf, 2, sizeof(buf) / 2, wav);
    //     read_total += read;
    // }
    // ESP_LOGI("AUDIO RECORDING", "END");

    // update_header_according_data_size(&header, read_total);
    // write_wav_header(wav, &header);
    // fclose(wav);
    // ESP_ERROR_CHECK(i2s_driver_uninstall(i2s_num));

    NetBuf_t* ftp_client_net_buf = NULL;
    FtpClient* ftpClient = getFtpClient();
    int retv;
    retv = ftpClient->ftpClientConnect("195.69.187.77", 21, &ftp_client_net_buf);

    if (retv) {
        ESP_LOGI("FTP_CLIENT", "CONNECTION SUCCESS");

        retv = ftpClient->ftpClientLogin("user23581", "mikkon24", ftp_client_net_buf);
        if (retv) {
            ESP_LOGI("FTP_CLIENT", "LOGGED IN");
            ftpClient->ftpClientPut(MOUNT_POINT"/TEST.WAV", "/test.wav", FTP_CLIENT_BINARY, ftp_client_net_buf); 

            ftpClient->ftpClientQuit(ftp_client_net_buf);
        } else {
            ESP_LOGE("FTP_CLIENT", "INVALID LOGIN");
        }
    } else {
        ESP_LOGE("FTP_CLIENT", "CONNECTION REFUSED");
    }
    ESP_ERROR_CHECK(esp_vfs_fat_sdmmc_unmount());
    gpio_set_level(GPIO_NUM_18, 1);
    ESP_LOGI("MAIN", "END");
}
