#pragma once
#include <esp_types.h>
#include "settings.h"
#include "list_of_errors.h"

#define DEVICE_ID_LEN sizeof("99999999")
#define FILE_NAME_LEN 64

typedef struct
{
    char str[DEVICE_ID_LEN];
} device_id;

typedef struct
{
    char str[FILE_NAME_LEN];
} file_name;

typedef struct
{
    settings _settings;
    float _battery_voltage;
    size_t _num_of_files;
    size_t _sound_lenght;
    uint64_t _free_space;
    uint64_t _total_space;
    list_of_errors _errors;
} info;


device_id get_device_id();

file_name get_wav_file_name(const char mount_point[]);

void init_rtc();

void sync_rtc();

struct tm get_time();

info get_all_info();

time_t get_time_now();

struct tm get_time_from_time(time_t time);
