#include <string.h>
#include "http_server.h"
#include "utils.h"
#include "list_of_errors.h"

void send_web_page(httpd_req_t *req) {
    info all_info = get_all_info();
    httpd_resp_sendstr_chunk(req, 
        "<!DOCTYPE html>"
        "<html lang=\"en\">"
        "<head>"
        "<meta charset=\"UTF-8\">"
        "<meta name=\"viewport\" , initial-scale=1.0>");
    device_id badge_id = get_device_id();
    char temp[256];
    snprintf(temp, sizeof(temp),
        "<title>Badge #%s</title>",
        badge_id.str);
    httpd_resp_sendstr_chunk(req, temp);
    httpd_resp_sendstr_chunk(req, "</head>");
    httpd_resp_sendstr_chunk(req, 
        "<body>"
        "<div style=\"height:90vh;overflow:auto;\">"
        "<table width=\"100%\">"
        "<tr><td valign=\"top\" width=\"40%\"><h1>Настройки</h1>");
    snprintf(temp, sizeof(temp),
        "<p>Пароль точки доступа Wi-Fi: <input type=\"password\" value=\"%s\"></p>",
        all_info._settings.wifi_AP_password);
    httpd_resp_sendstr_chunk(req, temp);
    snprintf(temp, sizeof(temp),
        "<p>Адрес FTP сервера: <input type=\"url\" value=\"%s\"></p>",
        all_info._settings.ftp_server_url);
    httpd_resp_sendstr_chunk(req, temp);
    snprintf(temp, sizeof(temp),
        "<p>Пароль FTP сервера: <input type=\"password\" value=\"%s\"></p>",
        all_info._settings.ftp_server_password);
    httpd_resp_sendstr_chunk(req, temp);
    httpd_resp_sendstr_chunk(req,
        "<p>Частота дискретизации звука:<select size=\"1\">");
    snprintf(temp, sizeof(temp),
        "<option %s value=\"8\">8 кГц</option>",
        all_info._settings.bits_per_sample == 8 ? "selected" : "");
    httpd_resp_sendstr_chunk(req, temp);
    snprintf(temp, sizeof(temp),
        "<option %s value=\"16\">16 кГц</option>",
        all_info._settings.bits_per_sample == 16 ? "selected" : "");
    httpd_resp_sendstr_chunk(req, temp);
    httpd_resp_sendstr_chunk(req,
        "</select></p><p>Разрядность звука:<select size=\"1\">");
    snprintf(temp, sizeof(temp),
        "<option %s value=\"8\">8 бит</option>",
        all_info._settings.sample_rate == 8 ? "selected" : "");
    httpd_resp_sendstr_chunk(req, temp);
    snprintf(temp, sizeof(temp),
        "<option %s value=\"16\">16 бит</option>",
        all_info._settings.sample_rate == 16 ? "selected" : "");
    httpd_resp_sendstr_chunk(req, temp);
    httpd_resp_sendstr_chunk(req,
        "</select></p><p>Длительность записываемых файлов:");
    snprintf(temp, sizeof(temp),
        "<input type=\"number\" min=\"0.5\" max=\"30\" step=\"0.5\" value=\"%.1f\">",
        (float)all_info._settings.rec_lenght_sec / 60);
    httpd_resp_sendstr_chunk(req, temp);
    httpd_resp_sendstr_chunk(req,
        "минут</p><h1>Статус устройства</h1>");
    snprintf(temp, sizeof(temp),
        "<p>Напряжение на аккумуляторе: <b>%.1fВ</b></p>",
        all_info._battery_voltage);
    httpd_resp_sendstr_chunk(req, temp);
    snprintf(temp, sizeof(temp),
        "<p>Количество файлов: <b>%d</b></p>",
        all_info._num_of_files);
    httpd_resp_sendstr_chunk(req, temp);
    snprintf(temp, sizeof(temp),
        "<p>Длительность записанного звука: <b>%.1f минут</b></p>",
        (float)all_info._sound_lenght / 60);
    httpd_resp_sendstr_chunk(req, temp);
    snprintf(temp, sizeof(temp),
        "<p>Занятое место на карте памяти / размер карты памяти: <b>%.1f/%.1f Мб</b></p>",
        (double)(all_info._free_space / 1024) / 1024,
        (double)(all_info._total_space / 1024) / 1024);
    httpd_resp_sendstr_chunk(req, temp);
    httpd_resp_sendstr_chunk(req,
        "</td>"
        "<td valign=\"top\" align=\"center\">"
        "<h1>Ошибки</h1>"
        "<div style=\"height:70vh;overflow:auto;\">"
        "<table border=\"1\" width=\"100%\">"
        "<tr>"
        "<th width=\"70%\" align=\"left\">Ошибка</th>"
        "<th align=\"left\">Время</th>"
        "</tr>");
    if (global_list_of_errors.lenght != 0) {
        error_node_t* tmp = global_list_of_errors.first;
        do {
            struct tm time_stamp = get_time_from_time(tmp->time);
            snprintf(temp, sizeof(temp),
                "<tr><td>%s</td><td>%02d:%02d:%02d %02d/%02d/%04d</td></tr>",
                get_error_desc(tmp->code).str,
                time_stamp.tm_hour, time_stamp.tm_min, time_stamp.tm_sec,
                time_stamp.tm_mday, time_stamp.tm_mon, time_stamp.tm_year);
            httpd_resp_sendstr_chunk(req, temp);
            tmp = tmp->next;
        } while(tmp != NULL);
    }
    
    httpd_resp_sendstr_chunk(req,
        "</table>"
        "</div>"
        "</td>"
        "</tr>"
        "</table>"
        "</div>"
        "<table width=\"100%\">"
        "<tr>"
        "<td width=\"20%\"><button>Сохранить</button></td>"
        "<td width=\"20%\"><button>Вернуть в заводские настройки</button></td>"
        "<td align=\"center\"><button>Стереть ошибки</button></td>"
        "</tr>"
        "</table>"
        "</body>"
        "</html>");
    httpd_resp_sendstr_chunk(req, NULL);
}
/* Our URI handler function to be called during GET /uri request */
esp_err_t get_handler(httpd_req_t *req)
{
    send_web_page(req);
    return ESP_OK;
}

/* Our URI handler function to be called during POST /uri request */
esp_err_t post_handler(httpd_req_t *req)
{
    /* Destination buffer for content of HTTP POST request.
     * httpd_req_recv() accepts char* only, but content could
     * as well be any binary data (needs type casting).
     * In case of string data, null termination will be absent, and
     * content length would give length of string */
    // char content[100];

    // /* Truncate if content length larger than the buffer */
    // size_t recv_size = MIN(req->content_len, sizeof(content));

    // int ret = httpd_req_recv(req, content, recv_size);
    // if (ret <= 0) {  /* 0 return value indicates connection closed */
    //     /* Check if timeout occurred */
    //     if (ret == HTTPD_SOCK_ERR_TIMEOUT) {
    //         /* In case of timeout one can choose to retry calling
    //          * httpd_req_recv(), but to keep it simple, here we
    //          * respond with an HTTP 408 (Request Timeout) error */
    //         httpd_resp_send_408(req);
    //     }
    //     /* In case of error, returning ESP_FAIL will
    //      * ensure that the underlying socket is closed */
    //     return ESP_FAIL;
    // }

    // /* Send a simple response */
    // // const char resp[] = "URI POST Response";
    // httpd_resp_send(req, content, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

/* URI handler structure for GET /uri */
httpd_uri_t uri_get = {
    .uri      = "/",
    .method   = HTTP_GET,
    .handler  = get_handler,
    .user_ctx = NULL
};

/* URI handler structure for POST /uri */
httpd_uri_t uri_post = {
    .uri      = "/uri",
    .method   = HTTP_POST,
    .handler  = post_handler,
    .user_ctx = NULL
};

/* Function for starting the webserver */
httpd_handle_t start_webserver(void)
{
    /* Generate default configuration */
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.stack_size = 8192;   
    /* Empty handle to esp_http_server */
    httpd_handle_t server = NULL;

    /* Start the httpd server */
    if (httpd_start(&server, &config) == ESP_OK) {
        /* Register URI handlers */
        httpd_register_uri_handler(server, &uri_get);
        httpd_register_uri_handler(server, &uri_post);
    }
    /* If server failed to start, handle will be NULL */
    return server;
}

/* Function for stopping the webserver */
void stop_webserver(httpd_handle_t server)
{
    if (server) {
        /* Stop the httpd server */
        httpd_stop(server);
    }
}
