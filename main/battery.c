#include <driver/adc.h>

#include "battery.h"

void init_battery() {
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(ADC1_CHANNEL_6, ADC_ATTEN_DB_11);
    esp_adc_cal_characterize(
        ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12,
        ESP_ADC_CAL_VAL_DEFAULT_VREF, &adc_characteristics);
}

float battery_charging_value() {
    uint32_t voltage_mv;
    esp_adc_cal_get_voltage(ADC_CHANNEL_6, &adc_characteristics, &voltage_mv);
    return ((float)voltage_mv * 2) / 1000;
}

charge_range get_char_range() {
    if (battery_charging_value() > 4.02f) {
        return MORE_THEN_80;
    } else if (battery_charging_value() > 3.75f) {
        return FROM_50_TO_80;
    } else if (battery_charging_value() > 3.525f) {
        return FROM_25_TO_50;
    } else {
        return LESS_THAN_25;
    }
}
