#include "sdmmc_cmd.h"
#include "driver/sdmmc_host.h"
#include "esp_vfs_fat.h"

#include "sdcard.h"
#include "pins.h"

sdmmc_card_t* enable_sd_card_vfs_fat() {
    gpio_reset_pin(SD_CARD_EN);
    gpio_set_direction(SD_CARD_EN, GPIO_MODE_OUTPUT_OD);
    gpio_set_level(SD_CARD_EN, 0);
    sdmmc_host_t host = SDMMC_HOST_DEFAULT();
    sdmmc_slot_config_t slot = SDMMC_SLOT_CONFIG_DEFAULT();
    slot.gpio_cd = SD_CARD_CD;
    esp_vfs_fat_mount_config_t mount_config = {
        .format_if_mount_failed = 1,
        .max_files = 2,
        .allocation_unit_size = 128 * FF_SS_SDCARD
    };
    sdmmc_card_t* card_info = NULL;
    ESP_ERROR_CHECK(esp_vfs_fat_sdmmc_mount(mount_point, &host, &slot, &mount_config, &card_info));
    return card_info;
}

void disable_sd_card_vfs_fat() {
    ESP_ERROR_CHECK(esp_vfs_fat_sdmmc_unmount());
    gpio_set_level(SD_CARD_EN, 1);
}

void get_space_on_sd_card(uint64_t* free_space, uint64_t* total_space) {
    FATFS *fs;
    DWORD free_clusters;
    int res = f_getfree(mount_point, &free_clusters, &fs);
    assert(res == FR_OK);
    uint64_t total_sectors = (uint64_t)(fs->n_fatent - 2) * fs->csize;
    uint64_t free_sectors = (uint64_t)free_clusters * fs->csize;

    // assuming the total size is < 4GiB, should be true for SPI Flash
    if (total_space != NULL) {
        *total_space = total_sectors * fs->ssize;
    }
    if (free_space != NULL) {
        *free_space = free_sectors * fs->ssize;
    }
}
