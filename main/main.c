#include "stdio.h"
// #include "string.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "sdmmc_cmd.h"
#include "driver/sdmmc_host.h"
#include "esp_vfs_fat.h"
#include "esp_log.h"

#include "rgb_led.h"
#include "input_gpio.h"


const static gpio_num_t RED_LED_PIN = GPIO_NUM_26;
const static gpio_num_t GREEN_LED_PIN = GPIO_NUM_9;
const static gpio_num_t BLUE_LED_PIN = GPIO_NUM_27;
const static gpio_num_t POWER_CONNECTION_PIN = GPIO_NUM_22;
const static gpio_num_t BUTTON_CONNECTION_PIN = GPIO_NUM_5;

void app_main(void)
{
    // esp_err_t ret = nvs_flash_init();
    // if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    //   ESP_ERROR_CHECK(nvs_flash_erase());
    //   ret = nvs_flash_init();
    // }
    // ESP_ERROR_CHECK(ret);
    // startSTA();

    // const int delay = 500 / portTICK_PERIOD_MS;
    rgb_gpio rgb_led = {
        .red = RED_LED_PIN,
        .green = GREEN_LED_PIN,
        .blue = BLUE_LED_PIN
    };
    init_led_gpio(rgb_led);
    // for(;;) {
    //     for (int color = 0; color < 3; color++) {
    //         en_led(((int*)(&rgb_led))[color]);
    //         ESP_LOGI("LED", "ENABLED");
    //         vTaskDelay(delay);
    //         dis_led(((int*)(&rgb_led))[color]);
    //         ESP_LOGI("LED", "DISABLED");
    //         vTaskDelay(delay);
    //     }
    // }

    input_config_t power = {
        .connection = POWER_CONNECTION_PIN,
        .enabled_state = 1
    };

    input_config_t button = {
        .connection = BUTTON_CONNECTION_PIN,
        .enabled_state = 0
    };

    input_floating_init(power);
    input_floating_init(button);
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

    // fflush(stdout);
    // fflush(stderr);

    // ESP_LOGI("START", "START");

    gpio_reset_pin(GPIO_NUM_18);
    gpio_set_direction(GPIO_NUM_18, GPIO_MODE_OUTPUT);
    gpio_set_level(GPIO_NUM_18, 0);
    sdmmc_host_t host = SDMMC_HOST_DEFAULT();
    sdmmc_slot_config_t slot = SDMMC_SLOT_CONFIG_DEFAULT();
    esp_vfs_fat_mount_config_t mount_config = {
        .format_if_mount_failed = 1,
        .max_files = 2,
        .allocation_unit_size = 128 * 512
    };
    sdmmc_card_t* card_info = NULL;
    #define MOUNT_POINT "/sdcard"
    ESP_ERROR_CHECK(esp_vfs_fat_sdmmc_mount("/sdcard", &host, &slot, &mount_config, &card_info));
    sdmmc_card_print_info(stdout, card_info);
    FILE* f = fopen(MOUNT_POINT"/hello.txt", "w");
    fprintf(f, "Hello world!\n");
    fclose(f);
    // f = fopen(MOUNT_POINT"/hello.txt", "r");
    // char file[sizeof("hello world!")];
    // fread(file, sizeof("hello world!"), sizeof("hello world!"), f);
    // ESP_LOGI("FILE", "%s\n", file);
    // vTaskDelay(10000 / portTICK_PERIOD_MS);
    ESP_LOGI("UNMOUNT", "BEFORE");
    ESP_ERROR_CHECK(esp_vfs_fat_sdcard_unmount("/sdcard", card_info));
    ESP_LOGI("UNMOUNT", "AFTER");
    gpio_set_level(GPIO_NUM_18, 1);
}
