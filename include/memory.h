/*
 * File: Memory.h
 * Author: Edouard Valenzuela
 *
 * Version: 1.0
 * Last Edited: May 15, 2025
 * Created on May 15, 2025
 *
 */

#include <stdio.h>
#include <string.h>
#include "driver/spi_master.h"
#include "driver/sdspi_host.h"
#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h" 


/**
* @brief Establish connection with the SD Card and the MCU
* 
* @return 
*     - ESP_OK on success
*     - ESP_ERR_XXX on failure
*/

esp_err_t sd_card_init(const char *mount_point, sdmmc_host_t host, sdmmc_card_t **card_handle);

/**
* @brief Create a new file and/or overwrite the information on it
* 
* @param path file path that is intended to be read from
* @param data information to be stored on that file
* @return 
*     - ESP_OK on success
*     - ESP_ERR_XXX on failure
*/


esp_err_t sd_card_write_file(const char *path, char *data);

/**
* @brief Read data from an existing file that is stored on the SD Card
* 
* @param path file path that is intended to be read from
*
* @return 
*     - ESP_OK on success
*     - ESP_ERR_XXX on failure
*/

esp_err_t sd_card_read_file(const char *path);

/**
* @brief Write additional information to either an existing file
* 
* @param path file path that is intended to be read from
* @param data information to be stored on that file
* @return 
*     - ESP_OK on success
*     - ESP_ERR_XXX on failure
*/
esp_err_t sd_card_append_file(const char *path, char *data);

/**
* @brief Permanently delete a file
* 
* @param path file path that is intended to be read from
*
* @return 
*     - ESP_OK on success
*     - ESP_ERR_XXX on failure
*/
esp_err_t sd_card_delete_file(const char *path);