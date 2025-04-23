/**
 * File: LoRa_interface.h
 * Author: Edouard Valenzuela (ecvalenz@ucsc.edu)
 * Brief: Library for interfacing with the ESP32 with the SX1262
 * Version: 1.0
 * Date: 2025-04-14
 * 
 * @copyright Copyright (c) 2025
 * 
 * 
 *  
 */

 #include "LoRa_driver.h"
 #include "driver/spi_master.h"

 /**
 * @brief  interface the spi bus with the sx1262 and add the device with respect to the ESP32 
 * @return status code
 *         - 1 success
 *         - 0 spi init failed
 * @note   none
 */
uint8_t esp32_SPI_bus_init(void);

 /**
 * @brief  free the spi bus from the sx1262 but first remove the device to do so
 * @return status code
 *         - 1 success
 *         - 0 spi deinit failed
 * @note   none
 */
uint8_t esp32_SPI_bus_deinit(void);

 /**
 * @brief  Read and write via SPI only one transaction 
 * @return status code
 *         - 1 success
 *         - 0 spi read and write asynch failed
 * @note   none
 */
uint8_t esp32_SPI_WRITE_READ_test(void);

 /**
 * @brief  Link the SPI bus the sx1262 and the ESP32 with respect to the ESP32 
 * @return status code
 *         - 1 success
 *         - 0 LoRa chip failed to initialize
 * @note   none
 */
uint8_t sx1262_device_init(void);

 /**
 * @brief  Deinitalize the LoRa
 * @return status code
 *         - 1 success
 *         - 0 LoRa chip failed to deinitialize
 * @note   none
 */
uint8_t sx1262_device_deinit(void);





