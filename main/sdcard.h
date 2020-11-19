#pragma once
#include <driver/sdmmc_types.h>
const static char mount_point[] = "/sdcard";

sdmmc_card_t* enable_sd_card_vfs_fat();

void disable_sd_card_vfs_fat();

void get_space_on_sd_card(uint64_t* free_space, uint64_t* total_space);