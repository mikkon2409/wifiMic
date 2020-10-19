#include "input_gpio.h"

void input_floating_init(input_config_t gpio) {
    gpio_reset_pin(gpio.connection);
    gpio_set_direction(gpio.connection, GPIO_MODE_INPUT);
    gpio_set_pull_mode(gpio.connection, GPIO_FLOATING);
}

int is_input_enabled(input_config_t gpio) {
    if (gpio.enabled_state)
        return gpio_get_level(gpio.connection);
    else
        return !gpio_get_level(gpio.connection);
}
