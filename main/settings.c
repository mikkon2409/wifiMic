#include <esp_log.h>
#include <nvs.h>
#include "settings.h"

#define STORAGE_NAMESPACE "storage"

settings get_default_settings() {
    settings result = {
        .wifi_AP_password = "123456789",
        .wifi_STA_SSID = "",
        .wifi_STA_password = "",
        .ftp_server_url = "",
        .ftp_server_login = "",
        .ftp_server_password = "",
        .sample_rate = 16000,
        .bits_per_sample = 16,
        .rec_lenght_sec = 60,
    };
    return result;
}

settings get_settings() {
    nvs_handle_t my_handle;

    nvs_open(STORAGE_NAMESPACE, NVS_READWRITE, &my_handle);
    settings ret;
    size_t required_size = 0;
    nvs_get_blob(my_handle, "settings", NULL, &required_size);
    if (required_size > 0) {
        nvs_get_blob(my_handle, "settings", &ret, &required_size);
        return ret;
    }
    return get_default_settings();
}

void set_settings(settings value) {
    nvs_handle_t my_handle;

    nvs_open(STORAGE_NAMESPACE, NVS_READWRITE, &my_handle);
    nvs_set_blob(my_handle, "settings", &value, sizeof(value));
    nvs_commit(my_handle);
    nvs_close(my_handle);
}

void erase_settings() {
    nvs_handle_t my_handle;

    nvs_open(STORAGE_NAMESPACE, NVS_READWRITE, &my_handle);
    nvs_erase_key(my_handle, "settings");
    nvs_commit(my_handle);
    nvs_close(my_handle);
}

void print_settings(settings tmp) {
    ESP_LOGI("SETTINGS", "%s:%s:%s:%s:%s:%s:%u:%hu:%d",
        tmp.wifi_AP_password, tmp.wifi_STA_SSID, tmp.wifi_STA_password,
        tmp.ftp_server_url, tmp.ftp_server_login, tmp.ftp_server_password,
        tmp.sample_rate, tmp.bits_per_sample, tmp.rec_lenght_sec);
}