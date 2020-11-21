#pragma once
#include <esp_adc_cal.h>

typedef enum {
    MORE_THEN_80,
    FROM_50_TO_80,
    FROM_25_TO_50,
    LESS_THAN_25
} charge_range;

esp_adc_cal_characteristics_t adc_characteristics;

void init_battery();

float battery_charging_value();

charge_range get_char_range();