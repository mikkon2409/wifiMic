#pragma once
#include <esp_types.h>
#include <time.h>

typedef enum {
    NO_ERROR = -1,
    CANT_CONNECT_STA,
    CANT_CONNECT_FTP,
    FTP_UPLOAD_ERROR,
    SDCARD_FULL,
} error_code;

typedef struct
{
    char str[128];
} error_desc;

struct error_node
{
    error_code code;
    time_t time;
    struct error_node* next;
};

typedef struct error_node error_node_t;

typedef struct
{
    error_node_t* first;
    error_node_t* last;
    size_t lenght;
} list_of_errors;

list_of_errors global_list_of_errors;

bool need_to_signal;

error_code last_error;

list_of_errors get_empty_list();

void add_error_to_list(list_of_errors* list, error_code code, time_t time);

void clean_list(list_of_errors* list);

list_of_errors get_list_of_errors();

error_desc get_error_desc(error_code code);