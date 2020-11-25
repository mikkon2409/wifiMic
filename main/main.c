#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <esp_log.h>
#include <esp_intr_alloc.h>
#include <esp_timer.h>

#include "nvs.h"
#include "rgb_led.h"
#include "input_gpio.h"
#include "esp_sntp.h"
#include "file_sender.h"
#include "track_rec.h"
#include "wifi.h"
#include "list_of_errors.h"
#include "utils.h"
#include "http_server.h"
#include "battery.h"

#define S_TO_US (1000 * 1000)

const static char TAG[] = "MAIN";

typedef enum {
    SLEEP,
    SETTINGS,
    RECORDING,
    UPLOADING
} state;

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

static volatile state global_state = SLEEP;

static volatile int64_t start_time = 0;
static volatile int64_t end_time = 0;

static volatile bool need_to_stop = false;

static xQueueHandle gpio_evt_queue = NULL;

static void IRAM_ATTR gpio_isr_handler(void* arg)
{
    uint32_t gpio_num = (uint32_t) arg;
    xQueueSendFromISR(gpio_evt_queue, &gpio_num, NULL);
}

static void gpio_task(void* arg)
{
    uint32_t io_num;
    for(;;) {
        if(xQueueReceive(gpio_evt_queue, &io_num, portMAX_DELAY)) {
            if (io_num == button.connection) {
                if (gpio_get_level(io_num) == button.enabled_state) {
                    start_time = esp_timer_get_time();
                } else {
                    end_time = esp_timer_get_time();
                    int64_t delta = end_time - start_time;
                    
                    if (global_state == SLEEP) {
                        if (delta < 1 * S_TO_US) {
                            global_state = RECORDING;
                            continue;
                        } else if (delta > 10 * S_TO_US) {
                            global_state = SETTINGS;
                            continue;
                        }
                    }

                    if (global_state == SETTINGS) {
                        if (delta > 10 * S_TO_US) {
                            global_state = SLEEP;
                            continue;
                        }
                    }

                    if (global_state == RECORDING) {
                        if (delta < 1 * S_TO_US) {
                            global_state = SLEEP;
                            need_to_stop = true;
                            continue;
                        }
                    }
                }
            }
            if (io_num == power.connection) {
                if (gpio_get_level(io_num) == power.enabled_state) {
                    if (global_state == SLEEP || global_state == RECORDING) {
                        global_state = UPLOADING;
                        continue;
                    }
                } else {
                    if (global_state == UPLOADING) {
                        global_state = SLEEP;
                        need_to_stop = true;
                        continue;
                    }
                }
            }
            printf("GPIO[%d] intr, val: %d\n", io_num, gpio_get_level(io_num));
        }
    }
}

static void led_task(void* arg) {
    state last_state = global_state;
    while(true) {
        if (last_state != global_state) {
            if (global_state == RECORDING) {
                en_led(rgb_led.green);
                vTaskDelay(5000 / portTICK_PERIOD_MS);
                dis_led(rgb_led.green);
            }
        } else {
            if (global_state == RECORDING) {
                for (int i = 0; i < 100 && !need_to_stop; ++i) {
                    vTaskDelay(100/ portTICK_PERIOD_MS);
                }
                charge_range charge_prc = get_char_range();
                for (int i = 0; i < charge_prc && !need_to_stop; ++i) {
                    en_led(rgb_led.green);
                    vTaskDelay(500 / portTICK_PERIOD_MS);
                    dis_led(rgb_led.green);
                    vTaskDelay(500 / portTICK_PERIOD_MS);
                }
            }
            if (global_state == UPLOADING) {
                for (int i = 0; i < 3 && !need_to_stop; ++i) {
                    en_led(rgb_led.blue);
                    vTaskDelay(500 / portTICK_PERIOD_MS);
                    dis_led(rgb_led.blue);
                    vTaskDelay(500 / portTICK_PERIOD_MS);
                }
                for (int i = 0; i < 100 && !need_to_stop; ++i) {
                    vTaskDelay(100/ portTICK_PERIOD_MS);
                }
            }
        }
        if (need_to_signal && global_state != SLEEP) {
            switch (last_error)
            {
            case CANT_CONNECT_STA:
                for (int i = 0; i < 1; ++i) {
                    en_led(rgb_led.red);
                    vTaskDelay(500 / portTICK_PERIOD_MS);
                    dis_led(rgb_led.red);
                    vTaskDelay(500 / portTICK_PERIOD_MS);
                }
                for (int i = 0; i < 100 && !need_to_stop; ++i) {
                    vTaskDelay(100/ portTICK_PERIOD_MS);
                }
                break;
            case CANT_CONNECT_FTP:
                for (int i = 0; i < 2; ++i) {
                    en_led(rgb_led.red);
                    vTaskDelay(500 / portTICK_PERIOD_MS);
                    dis_led(rgb_led.red);
                    vTaskDelay(500 / portTICK_PERIOD_MS);
                }
                for (int i = 0; i < 100 && !need_to_stop; ++i) {
                    vTaskDelay(100/ portTICK_PERIOD_MS);
                }
                break;
            case FTP_UPLOAD_ERROR:
                for (int i = 0; i < 3; ++i) {
                    en_led(rgb_led.red);
                    vTaskDelay(500 / portTICK_PERIOD_MS);
                    dis_led(rgb_led.red);
                    vTaskDelay(500 / portTICK_PERIOD_MS);
                }
                for (int i = 0; i < 100 && !need_to_stop; ++i) {
                    vTaskDelay(100/ portTICK_PERIOD_MS);
                }
                break;
            case SDCARD_FULL:
                for (int i = 0; i < 4 && !need_to_stop; ++i) {
                    en_led(rgb_led.blue);
                    vTaskDelay(500 / portTICK_PERIOD_MS);
                    dis_led(rgb_led.blue);
                    vTaskDelay(500 / portTICK_PERIOD_MS);
                }
                for (int i = 0; i < 100 && !need_to_stop; ++i) {
                    vTaskDelay(100/ portTICK_PERIOD_MS);
                }
                break;
            default:
                break;
            }
        }
        last_state = global_state;
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

void app_main(void)
{
    ESP_LOGI(TAG, "INIT START");
    global_list_of_errors = get_empty_list();
    need_to_signal = false;
    last_error = NO_ERROR;
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
    gpio_evt_queue = xQueueCreate(1, sizeof(uint32_t));
    //start gpio task
    xTaskCreate(gpio_task, "gpio_task", 2048, NULL, 10, NULL);
    xTaskCreate(led_task, "red_led_task", 2048, NULL, 10, NULL);

    //hook isr handler for specific gpio pin
    gpio_isr_handler_add(button.connection, gpio_isr_handler, (void*) button.connection);
    //hook isr handler for specific gpio pin
    gpio_isr_handler_add(power.connection, gpio_isr_handler, (void*) power.connection);
    add_error_to_list(&global_list_of_errors, -1, get_time_now());
    add_error_to_list(&global_list_of_errors, 2, get_time_now());
    ESP_LOGI(TAG, "INIT FINISH");

    state last_state = global_state;
    httpd_handle_t web_handler = NULL;
    while(true) {
        if (global_state == RECORDING) {
            error_code ret = rec_track(&need_to_stop);
            if (ret == SDCARD_FULL)
                add_error_to_list(&global_list_of_errors, ret, get_time_now());
            if (global_state == RECORDING)
                global_state = SLEEP;
            need_to_stop = false;
        }
        if (global_state == UPLOADING) {
            error_code ret = NO_ERROR;
            ret = startSTA();
            if (ret == NO_ERROR) {
                sync_rtc();
                ret = send_all_files(&need_to_stop);
                if (ret == FTP_UPLOAD_ERROR || ret == CANT_CONNECT_FTP)
                    add_error_to_list(&global_list_of_errors, ret, get_time_now());
            } else if (ret == CANT_CONNECT_STA) {
                add_error_to_list(&global_list_of_errors, ret, get_time_now());
            }
            stopSTA();
            if (global_state == UPLOADING)
                global_state = SLEEP;
            need_to_stop = false;
        }
        if (last_state != SETTINGS && global_state == SETTINGS) {
            startAP();
            web_handler = start_webserver();
        }
        if (last_state == SETTINGS && global_state != SETTINGS) {
            stopAP();
            stop_webserver(web_handler);
            web_handler = NULL;
        }
        last_state = global_state;
        vTaskDelay(200 / portTICK_PERIOD_MS);
    }
}
