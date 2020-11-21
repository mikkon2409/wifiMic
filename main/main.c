#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_log.h>

#include "nvs.h"
#include "rgb_led.h"
#include "input_gpio.h"
#include "utils.h"
#include "esp_sntp.h"
#include "file_sender.h"
#include "track_rec.h"
#include "wifi.h"
#include "list_of_errors.h"
#include "http_server.h"
#include "pins.h"
#include "battery.h"

const static char TAG[] = "MAIN";
static volatile bool need_to_stop = false;

void app_main(void)
{
    ESP_LOGI(TAG, "INIT START");
    global_list_of_errors = get_empty_list();
    rgb_gpio rgb_led = {
        .red = RED_LED_PIN,
        .green = GREEN_LED_PIN,
        .blue = BLUE_LED_PIN
    };

    input_config_t power = {
        .connection = POWER_PIN,
        .enabled_state = 1
    };

    input_config_t button = {
        .connection = BUTTON_PIN,
        .enabled_state = 0
    };

    input_floating_init(power);
    input_floating_init(button);
    init_led_gpio(rgb_led);
    init_battery();
    init_nvs();
    init_rtc();
    add_error_to_list(&global_list_of_errors, -1, get_time_now());
    add_error_to_list(&global_list_of_errors, 2, get_time_now());
    add_error_to_list(&global_list_of_errors, 1, get_time_now());
    add_error_to_list(&global_list_of_errors, 0, get_time_now());
    add_error_to_list(&global_list_of_errors, 3, get_time_now());
    add_error_to_list(&global_list_of_errors, 6, get_time_now());
    ESP_LOGI(TAG, "INIT FINISH");

    ESP_LOGI(TAG, "APP START");
    startSTA();
    sync_rtc();
    start_webserver();
    ESP_LOGI(TAG, "APP FINISH");


    // const int delay = 500 / portTICK_PERIOD_MS;
    
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


    
    



}
