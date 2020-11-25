#include <stdlib.h>
#include <string.h>

#include "list_of_errors.h"

list_of_errors get_empty_list() {
    list_of_errors list = {
        .first = NULL,
        .last = NULL,
        .lenght = 0
    };
    return list;
}

void add_error_to_list(list_of_errors* list, error_code code, time_t time) {
    error_node_t* new_node = (error_node_t*)malloc(sizeof(error_node_t));
    new_node->code = code;
    new_node->time = time;
    new_node->next = NULL;
    if (list->lenght == 0) {
        list->first = new_node;
        list->last = new_node;
    } else {
        list->last->next = new_node;
        list->last = new_node;
    }
    list->lenght += 1;
    last_error = code;
    need_to_signal = true;
}

void clean_list(list_of_errors* list) {
    if (list->lenght == 0)
        return;
    if (list->first == list->last) {
        free(list->first);
    } else {
        error_node_t* tmp = list->first;
        while (tmp->next != NULL)
        {
            error_node_t* tmp1 = tmp->next;
            free(tmp);
            tmp = tmp1;
        }
    }
    list->first = NULL;
    list->last = NULL;
    list->lenght = 0;
}

list_of_errors get_list_of_errors() {
    return global_list_of_errors;
}

error_desc get_error_desc(error_code code) {
    error_desc desc;
    switch (code)
    {
    case CANT_CONNECT_STA:
        strcpy(desc.str, "Не удалось подключиться к точке доступа Wi-Fi");
        break;
    case CANT_CONNECT_FTP:
        strcpy(desc.str, "Ошибка подключения к FTP серверу");
        break;
    case FTP_UPLOAD_ERROR:
        strcpy(desc.str, "Ошибка передачи файла на FTP сервер");
        break;
    case SDCARD_FULL:
        strcpy(desc.str, "Карта памяти заполнена");
        break;
    default:
        strcpy(desc.str, "Ошибка общего характера");
        break;
    }
    return desc;
}