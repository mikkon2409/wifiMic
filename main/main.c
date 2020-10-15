#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

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
    while(1) {
        if (is_input_enabled(power)) {
            en_led(rgb_led.red);
        } else {
            dis_led(rgb_led.red);
        }
        if (is_input_enabled(button)) {
            en_led(rgb_led.blue);
        } else {
            dis_led(rgb_led.blue);
        }
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}
