#include "rgb_led.h"

void en_led(gpio_num_t gpio) {
    gpio_set_level(gpio, ENABLED_LED);
}

void dis_led(gpio_num_t gpio) {
    gpio_set_level(gpio, DISABLED_LED);
}

void init_led_gpio(rgb_gpio gpio) {
    gpio_reset_pin(gpio.red);
    // gpio_reset_pin(gpio.green);
    gpio_reset_pin(gpio.blue);

    gpio_set_direction(gpio.red,   GPIO_MODE_OUTPUT);
    // gpio_set_direction(gpio.green, GPIO_MODE_OUTPUT);
    gpio_set_direction(gpio.blue,  GPIO_MODE_OUTPUT);

    dis_led(gpio.red);
    // dis_led(gpio.green);
    dis_led(gpio.blue);
}

