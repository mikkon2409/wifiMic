#include <esp_system.h>
#include <stdio.h>
#include <stdlib.h>
#include <esp_log.h>
#include <esp_sntp.h>
#include <time.h>
#include <sys/dirent.h>

#include "utils.h"
#include "sdcard.h"
#include "wav.h"
#include "battery.h"
const static char TAG[] = "UTILS";


int32_t get_device_id_num() {
    uint8_t mac_addr[6];
    ESP_ERROR_CHECK(esp_efuse_mac_get_default(mac_addr));
    int32_t device_id = 0;
    device_id |= (int32_t)(mac_addr[3] << 16 | mac_addr[4] << 8 | mac_addr[5] << 0);
    return device_id;
}

device_id get_device_id() {
    const int32_t device_id_num = get_device_id_num();
    device_id result;
    snprintf(result.str, DEVICE_ID_LEN, "%08d", device_id_num);
    ESP_LOGI(TAG, "device_id: %s", result.str);
    return result;
}

void init_rtc() {
    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_setservername(0, "pool.ntp.org");
    sntp_init();

    setenv("TZ", "MSK-3MSD,M3.5.0/2,M10.5.0/3", 1);
    tzset();
}

void sync_rtc() {
    sntp_restart();
    int retry = 0;
    const int retry_count = 10;
    while (sntp_get_sync_status() == SNTP_SYNC_STATUS_RESET && ++retry < retry_count) {
        ESP_LOGI(TAG, "Waiting for system time to be set... (%d/%d)", retry, retry_count);
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
}

time_t get_time_now() {
    time_t now;
    time(&now);
    return now;
}

struct tm get_time_from_time(time_t time) {
    struct tm timeinfo;
    localtime_r(&time, &timeinfo); 
    timeinfo.tm_year += 1900;
    return timeinfo;
}

struct tm get_time() {
    return get_time_from_time(get_time_now());
}

file_name get_wav_file_name(const char mount_point[]) {
    struct tm timeinfo = get_time();
    device_id id = get_device_id();
    file_name name;
    ESP_LOGI(TAG, "%d %d %d", timeinfo.tm_year, timeinfo.tm_mon, timeinfo.tm_mday);
    snprintf(name.str, FILE_NAME_LEN, "%s/%04d_%02d_%02d_%02d_%02d_%02d_%s.wav",
        mount_point,
        timeinfo.tm_year, timeinfo.tm_mon, timeinfo.tm_mday,
        timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec,
        id.str);
    ESP_LOGI(TAG, "%s", name.str);
    return name;
}

float get_battery_voltage() {
    return battery_charging_value();
}

size_t get_num_of_files() {
    DIR* dir = opendir(mount_point);
    size_t count = 0;
    while (readdir(dir) != NULL)
        count += 1;
    closedir(dir);
    return count;
}

// calculate time of sound in seconds
size_t get_sound_lenght() {
    DIR* dir = opendir(mount_point);
    struct dirent *ent;
    size_t total_len = 0;
    while ((ent = readdir(dir)) != NULL) {
        char fs_path[300] = "";
        snprintf(fs_path, sizeof(fs_path), "%s/%s", mount_point, ent->d_name);
        FILE* file = fopen(fs_path, "rb");
        wav_header header;
        if (fread(&header, sizeof(header), 1, file) == 1) {
            total_len += header.subchank2Size / header.byteRate;
        }
        fclose(file);
    }
    closedir(dir);
    return total_len;
}

info get_all_info() {
    info res;
    res._battery_voltage = get_battery_voltage();
    enable_sd_card_vfs_fat();
    get_space_on_sd_card(&res._free_space, &res._total_space);
    res._num_of_files = get_num_of_files();
    res._settings = get_settings();
    res._sound_lenght = get_sound_lenght();
    disable_sd_card_vfs_fat();
    return res;
}
