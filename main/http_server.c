#include <string.h>
#include "http_server.h"
#include "utils.h"
#include "list_of_errors.h"
#include <esp_log.h>

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
    extern const char script_start[] asm("_binary_script_html_start");
    extern const char script_end[]   asm("_binary_script_html_end");
    const size_t script_size = (script_end - script_start);
    httpd_resp_send_chunk(req, script_start, script_size);
    httpd_resp_sendstr_chunk(req, "</head>");
    httpd_resp_sendstr_chunk(req, 
        "<body>"
        "<div style=\"height:90vh;overflow:auto;\">"
        "<table width=\"100%\">"
        "<tr><td valign=\"top\" width=\"40%\"><h1>Настройки</h1>");
    snprintf(temp, sizeof(temp),
        "<p>Пароль точки доступа Wi-Fi бейджа: <input type=\"password\" id=\"badge_pass\" value=\"%s\"></p>",
        all_info._settings.wifi_AP_password);
    httpd_resp_sendstr_chunk(req, temp);
    snprintf(temp, sizeof(temp),
        "<p>Имя Wi-Fi станции: <input type=\"text\" id=\"wifi_ssid\" value=\"%s\"></p>",
        all_info._settings.wifi_STA_SSID);
    httpd_resp_sendstr_chunk(req, temp);
    snprintf(temp, sizeof(temp),
        "<p>Пароль Wi-Fi станции: <input type=\"password\" id=\"wifi_pass\" value=\"%s\"></p>",
        all_info._settings.wifi_STA_password);
    httpd_resp_sendstr_chunk(req, temp);
    snprintf(temp, sizeof(temp),
        "<p>Адрес FTP сервера: <input type=\"url\" id=\"ftp_url\" value=\"%s\"></p>",
        all_info._settings.ftp_server_url);
    httpd_resp_sendstr_chunk(req, temp);
    snprintf(temp, sizeof(temp),
        "<p>Логин FTP сервера: <input type=\"text\" id=\"ftp_login\" value=\"%s\"></p>",
        all_info._settings.ftp_server_login);
    httpd_resp_sendstr_chunk(req, temp);
    snprintf(temp, sizeof(temp),
        "<p>Пароль FTP сервера: <input type=\"password\" id=\"ftp_pass\" value=\"%s\"></p>",
        all_info._settings.ftp_server_password);
    httpd_resp_sendstr_chunk(req, temp);
    httpd_resp_sendstr_chunk(req,
        "<p>Частота дискретизации звука:<select size=\"1\" id=\"sample_rate\">");
    snprintf(temp, sizeof(temp),
        "<option %s value=\"8000\">8 кГц</option>",
        all_info._settings.sample_rate == 8000 ? "selected" : "");
    httpd_resp_sendstr_chunk(req, temp);
    snprintf(temp, sizeof(temp),
        "<option %s value=\"16000\">16 кГц</option>",
        all_info._settings.sample_rate == 16000 ? "selected" : "");
    httpd_resp_sendstr_chunk(req, temp);
    httpd_resp_sendstr_chunk(req,
        "</select></p><p>Разрядность звука:<select size=\"1\" id=\"byte_rate\">");
    snprintf(temp, sizeof(temp),
        "<option %s value=\"8\">8 бит</option>",
        all_info._settings.bits_per_sample == 8 ? "selected" : "");
    httpd_resp_sendstr_chunk(req, temp);
    snprintf(temp, sizeof(temp),
        "<option %s value=\"16\">16 бит</option>",
        all_info._settings.bits_per_sample == 16 ? "selected" : "");
    httpd_resp_sendstr_chunk(req, temp);
    httpd_resp_sendstr_chunk(req,
        "</select></p><p>Длительность записываемых файлов:");
    snprintf(temp, sizeof(temp),
        "<input type=\"number\" id=\"lenght\" min=\"0.5\" max=\"30\" step=\"0.5\" value=\"%.1f\">",
        (double)all_info._settings.rec_lenght_sec / 60);
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
        "<td width=\"20%\"><button onclick=\"set_settings()\">Сохранить</button></td>"
        "<td width=\"20%\"><button onclick=\"default_settings()\">Вернуть в заводские настройки</button></td>"
        "<td align=\"center\"><button onclick=\"erase()\">Стереть ошибки</button></td>"
        "</tr>"
        "</table>");
    httpd_resp_sendstr_chunk(req,
        "</body>"
        "</html>");
    httpd_resp_sendstr_chunk(req, NULL);
}

esp_err_t get_handler(httpd_req_t *req)
{
    send_web_page(req);
    return ESP_OK;
}

esp_err_t save_handler(httpd_req_t *req)
{
    char content[255];

    if (req->content_len + 1 <= sizeof(content)){
        int ret = httpd_req_recv(req, content, req->content_len);
        if (ret <= 0) {  /* 0 return value indicates connection closed */
            /* Check if timeout occurred */
            if (ret == HTTPD_SOCK_ERR_TIMEOUT) {
                /* In case of timeout esp_log_timestampone can choose to retry calling
                * httpd_req_recv(), but to keep it simple, here we
                * respond with an HTTP 408 (Request Timeout) error */
                httpd_resp_send_408(req);
            }
            /* In case of error, returning ESP_FAIL will
            * ensure that the underlying socket is closed */
            return ESP_FAIL;
        }
        settings tmp;
        char resp[255];
        content[req->content_len] = '\0';
        ESP_LOGI("HTTPD POST SAVE", "%s", content);
        char* tmp_char1 = content;
        char* tmp_char = strchr(tmp_char1, '\t');
        int len = tmp_char - tmp_char1;
        if (len + 1 > sizeof(tmp.wifi_AP_password)) {
            snprintf(resp, sizeof(resp),
                "Ошибка сохранения настроек!\nМаксимальная длина пароля точки доступа %d символов!",
                sizeof(tmp.wifi_AP_password) - 1);
            httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
            return ESP_OK;
        }
        memcpy(tmp.wifi_AP_password, tmp_char1, len);
        tmp.wifi_AP_password[len] = '\0';

        tmp_char1 = &tmp_char[1];
        tmp_char = strchr(tmp_char1, '\t');
        len = tmp_char - tmp_char1;
        if (len + 1 > sizeof(tmp.wifi_STA_SSID)) {
            snprintf(resp, sizeof(resp),
                "Ошибка сохранения настроек!\nМаксимальная длина имени Wi-Fi станции %d символов!",
                sizeof(tmp.wifi_STA_SSID) - 1);
            httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
            return ESP_OK;
        }
        memcpy(tmp.wifi_STA_SSID, tmp_char1, len);
        tmp.wifi_STA_SSID[len] = '\0';

        tmp_char1 = &tmp_char[1];
        tmp_char = strchr(tmp_char1, '\t');
        len = tmp_char - tmp_char1;
        if (len + 1 > sizeof(tmp.wifi_STA_password)) {
            snprintf(resp, sizeof(resp),
                "Ошибка сохранения настроек!\nМаксимальная длина пароля Wi-Fi станции %d символов!",
                sizeof(tmp.wifi_STA_password) - 1);
            httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
            return ESP_OK;
        }
        memcpy(tmp.wifi_STA_password, tmp_char1, len);
        tmp.wifi_STA_password[len] = '\0';

        tmp_char1 = &tmp_char[1];
        tmp_char = strchr(tmp_char1, '\t');
        len = tmp_char - tmp_char1;
        if (len + 1 > sizeof(tmp.ftp_server_url)) {
            snprintf(resp, sizeof(resp),
                "Ошибка сохранения настроек!\nМаксимальная длина адреса FTP сервера %d символов!",
                sizeof(tmp.ftp_server_url) - 1);
            httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
            return ESP_OK;
        }
        memcpy(tmp.ftp_server_url, tmp_char1, len);
        tmp.ftp_server_url[len] = '\0';

        tmp_char1 = &tmp_char[1];
        tmp_char = strchr(tmp_char1, '\t');
        len = tmp_char - tmp_char1;
        if (len + 1 > sizeof(tmp.ftp_server_login)) {
            snprintf(resp, sizeof(resp),
                "Ошибка сохранения настроек!\nМаксимальная длина логина FTP сервера %d символов!",
                sizeof(tmp.ftp_server_login) - 1);
            httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
            return ESP_OK;
        }
        memcpy(tmp.ftp_server_login, tmp_char1, len);
        tmp.ftp_server_login[len] = '\0';

        tmp_char1 = &tmp_char[1];
        tmp_char = strchr(tmp_char1, '\t');
        len = tmp_char - tmp_char1;
        if (len + 1 > sizeof(tmp.ftp_server_password)) {
            snprintf(resp, sizeof(resp),
                "Ошибка сохранения настроек!\nМаксимальная длина пароля FTP сервера %d символов!",
                sizeof(tmp.ftp_server_password) - 1);
            httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
            return ESP_OK;
        }
        memcpy(tmp.ftp_server_password, tmp_char1, len);
        tmp.ftp_server_password[len] = '\0';

        tmp_char1 = &tmp_char[1];
        int num_of_fields = sscanf(tmp_char1, "%u\t%hu\t%d",
            &tmp.sample_rate, &tmp.bits_per_sample, &tmp.rec_lenght_sec);
        if (num_of_fields != 3) {
            snprintf(resp, sizeof(resp),
                "Ошибка сохранения настроек!\nПроверьте настройки параметров звука!");
            httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
            return ESP_OK;
        }
        set_settings(tmp);
        snprintf(resp, sizeof(resp), "Настройки сохранены!");
        httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
    } else {
        const char resp[] = "Ошибка сохранения настроек!\nУменьшите количество символов в полях!";
        httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
    }
    return ESP_OK;
}

esp_err_t default_settings_handler(httpd_req_t *req) {
    erase_settings();
    httpd_resp_sendstr(req, "");
    return ESP_OK;
}

esp_err_t erase_errors_handler(httpd_req_t *req) {
    clean_list(&global_list_of_errors);
    httpd_resp_sendstr(req, "");
    return ESP_OK;
}

httpd_uri_t uri_get = {
    .uri      = "/",
    .method   = HTTP_GET,
    .handler  = get_handler,
    .user_ctx = NULL
};

httpd_uri_t uri_save = {
    .uri      = "/save",
    .method   = HTTP_POST,
    .handler  = save_handler,
    .user_ctx = NULL
};

httpd_uri_t uri_default = {
    .uri      = "/default",
    .method   = HTTP_GET,
    .handler  = default_settings_handler,
    .user_ctx = NULL
};

httpd_uri_t uri_erase_error = {
    .uri      = "/erase",
    .method   = HTTP_GET,
    .handler  = erase_errors_handler,
    .user_ctx = NULL
};

httpd_handle_t start_webserver(void)
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.stack_size = 12000;
    httpd_handle_t server = NULL;

    if (httpd_start(&server, &config) == ESP_OK) {
        httpd_register_uri_handler(server, &uri_get);
        httpd_register_uri_handler(server, &uri_save);
        httpd_register_uri_handler(server, &uri_default);
        httpd_register_uri_handler(server, &uri_erase_error);
    }
    /* If server failed to start, handle will be NULL */
    return server;
}

void stop_webserver(httpd_handle_t server)
{
    if (server) {
        httpd_stop(server);
    }
}
