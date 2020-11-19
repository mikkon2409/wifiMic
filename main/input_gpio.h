#pragma once
#include <driver/gpio.h>

typedef struct {
    gpio_num_t connection;
    int enabled_state;
} input_config_t;

void input_floating_init(input_config_t gpio);

int is_input_enabled(input_config_t gpio);
