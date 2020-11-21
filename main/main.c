#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <esp_log.h>
#include <esp_intr_alloc.h>

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
#include "battery.h"

const static char TAG[] = "MAIN";

rgb_gpio rgb_led = {
        .red = GPIO_NUM_26,
        .green = GPIO_NUM_19,
        .blue = GPIO_NUM_27
    };

input_config_t power = {
    .connection = GPIO_NUM_22,
    .enabled_state = 1
};

input_config_t button = {
    .connection = GPIO_NUM_5,
    .enabled_state = 0
};

static volatile bool need_to_stop = false;

static xQueueHandle gpio_evt_queue = NULL;

static void IRAM_ATTR gpio_isr_handler(void* arg)
{
    uint32_t gpio_num = (uint32_t) arg;
    xQueueSendFromISR(gpio_evt_queue, &gpio_num, NULL);
}

static void gpio_task_example(void* arg)
{
    uint32_t io_num;
    for(;;) {
        if(xQueueReceive(gpio_evt_queue, &io_num, portMAX_DELAY)) {
            printf("GPIO[%d] intr, val: %d\n", io_num, gpio_get_level(io_num));
        }
    }
}

void app_main(void)
{
    ESP_LOGI(TAG, "INIT START");
    global_list_of_errors = get_empty_list();
    

    input_floating_init(power);
    input_floating_init(button);
    init_led_gpio(rgb_led);
    init_battery();
    init_nvs();
    init_rtc();
    gpio_set_intr_type(button.connection, GPIO_INTR_ANYEDGE);
    gpio_set_intr_type(power.connection, GPIO_INTR_ANYEDGE);
    gpio_install_isr_service(0);


    //create a queue to handle gpio event from isr
    gpio_evt_queue = xQueueCreate(10, sizeof(uint32_t));
    //start gpio task
    xTaskCreate(gpio_task_example, "gpio_task_example", 2048, NULL, 10, NULL);

    //hook isr handler for specific gpio pin
    gpio_isr_handler_add(button.connection, gpio_isr_handler, (void*) button.connection);
    //hook isr handler for specific gpio pin
    gpio_isr_handler_add(power.connection, gpio_isr_handler, (void*) power.connection);
    // add_error_to_list(&global_list_of_errors, -1, get_time_now());
    // add_error_to_list(&global_list_of_errors, 2, get_time_now());
    ESP_LOGI(TAG, "INIT FINISH");

    ESP_LOGI(TAG, "APP START");

    ESP_LOGI(TAG, "DEVICE_ID: %s", get_device_id().str);
    // startSTA();
    // sync_rtc();
    // start_webserver();
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
