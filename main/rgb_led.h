#pragma once
#include "driver/gpio.h"

#define ENABLED_LED 0
#define DISABLED_LED (!ENABLED_LED)

typedef struct {
    gpio_num_t red;
    gpio_num_t green;
    gpio_num_t blue;
} rgb_gpio;

void en_led(gpio_num_t gpio);

void dis_led(gpio_num_t gpio);

void init_led_gpio(rgb_gpio gpio);
