#include <esp_log.h>
#include <sys/dirent.h>
#include <stdio.h>
#include "file_sender.h"
#include "settings.h"
#include "ftp_client.h"
#include "sdcard.h"


error_code send_all_files(volatile bool* need_to_stop) {
    struct dirent *ent;
    settings sett = get_settings();
    enable_sd_card_vfs_fat();
    DIR* dir = opendir(mount_point);
    NetBuf_t* ftp_client_net_buf = NULL;
    FtpClient* ftpClient = getFtpClient();
    int retv;
    retv = ftpClient->ftpClientConnect(sett.ftp_server_url, 21, &ftp_client_net_buf);
    error_code error = NO_ERROR;
    if (retv) {
        ESP_LOGI("FTP_CLIENT", "CONNECTION SUCCESS");

        retv = ftpClient->ftpClientLogin(
                                        sett.ftp_server_login, 
                                        sett.ftp_server_password, 
                                        ftp_client_net_buf);
        if (retv) {
            ESP_LOGI("FTP_CLIENT", "LOGGED IN");
            while ((ent = readdir (dir)) != NULL && !*need_to_stop) {
                char fs_path[300] = "";
                char ftp_path[300] = "";
                snprintf(fs_path, sizeof(fs_path), "%s/%s", mount_point, ent->d_name);
                snprintf(ftp_path, sizeof(ftp_path), "/%s", ent->d_name);
                bool sended = false;
                if(ftpClient->ftpClientPut(fs_path, ftp_path, FTP_CLIENT_BINARY, ftp_client_net_buf)) {
                    remove(fs_path);
                    sended = true;
                    ESP_LOGI("FTP_CLIENT", "%s uploaded", ent->d_name);
                }
                if (!sended)
                    if(ftpClient->ftpClientPut(fs_path, ftp_path, FTP_CLIENT_BINARY, ftp_client_net_buf)) {
                        remove(fs_path);
                        sended = true;
                        ESP_LOGI("FTP_CLIENT", "%s uploaded", ent->d_name);
                    }
                if (!sended)
                    if(ftpClient->ftpClientPut(fs_path, ftp_path, FTP_CLIENT_BINARY, ftp_client_net_buf)) {
                        remove(fs_path);
                        sended = true;
                        ESP_LOGI("FTP_CLIENT", "%s uploaded", ent->d_name);
                    } else {
                        error = FTP_UPLOAD_ERROR;
                        break;
                    }
            }

            ftpClient->ftpClientQuit(ftp_client_net_buf);
        } else {
            error = CANT_CONNECT_FTP;
            ESP_LOGE("FTP_CLIENT", "INVALID LOGIN");
        }
    } else {
        error = CANT_CONNECT_FTP;
        ESP_LOGE("FTP_CLIENT", "CONNECTION REFUSED");
    }

    closedir(dir);
    disable_sd_card_vfs_fat();
    return error;
}