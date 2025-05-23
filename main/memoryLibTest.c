/**
 * @file memoryLibTest.c
 * @author Edouard Valenzuela
 * @brief Verification of memory library
 * @version 0.1
 * @date 2025-05-22
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include "../include/memory.h"

static const char *TAG = "memoryLibTest";
#define EXAMPLE_MAX_CHAR_SIZE    64
#define MOUNT_POINT "/sdcard"

void app_main(void)
{
    esp_err_t ret;

    sdmmc_card_t *card;
    const char mount_point[] = MOUNT_POINT;
    sdmmc_host_t host = SDSPI_HOST_DEFAULT();

	if(sd_card_init(mount_point, host, &card) != ESP_OK)
    {
        ESP_LOGE(TAG, "SD Card failed to be initialized");
        return;
    }

    const char *file_helly = MOUNT_POINT"/helly.txt";
    char data[EXAMPLE_MAX_CHAR_SIZE];
    snprintf(data, EXAMPLE_MAX_CHAR_SIZE, "%s %s!\n", "What the helly", card->cid.name);

    ret = sd_card_write_file(file_helly, data);
    if (ret != ESP_OK) 
    {
        ESP_LOGE(TAG, "Failed to write to helly.txt");
        return;
    }

    ret = sd_card_read_file(file_helly);
    if (ret != ESP_OK) 
    {
        ESP_LOGE(TAG, "Failed to read from helly.txt");
        return;
    }

    char newline_data[EXAMPLE_MAX_CHAR_SIZE];
    snprintf(newline_data, EXAMPLE_MAX_CHAR_SIZE, "%s %s!\n", "What the Helly Bron James", card->cid.name);
    ret = sd_card_append_file(file_helly, newline_data);

    if (ret != ESP_OK) 
    {
        ESP_LOGE(TAG, "Failed to append helly.txt");
        return;
    }

    //Deinitialize the sd card by unmounting the partition and deinitializing the SPI Bus
    ret = esp_vfs_fat_sdcard_unmount(mount_point, card);
    if(ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Card failed to unmount");
        return;
    }

    ESP_LOGI(TAG, "Card unmounted");

    //deinitialize the bus after all devices are removed
    ret = spi_bus_free(host.slot);

    if(ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to deinit SPI Bus");
        return;
    }

	while(1)
    {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
