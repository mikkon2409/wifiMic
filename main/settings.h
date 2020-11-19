#pragma once
#include <esp_types.h>

typedef struct
{
    char wifi_STA_SSID[32];
    char wifi_STA_password[32];
    char wifi_AP_password[32];
    char ftp_server_url[64];
    char ftp_server_login[32];
    char ftp_server_password[32];
    uint32_t sample_rate;
    uint16_t bits_per_sample;
    int32_t rec_lenght_sec;
} settings;

settings get_settings();

void set_settings(settings value);

void erase_settings();
