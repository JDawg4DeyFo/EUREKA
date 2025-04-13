/**
 * File: LoRa_interface.h
 * Author: Edouard Valenzuela (ecvalenz@ucsc.edu)
 * Brief: Library for interfacing with the ESP32 with the SX1262
 * Version: 0.2
 * Date: 2025-04-12
 * 
 * @copyright Copyright (c) 2025
 * 
 * 
 *  
 */

 #include "LoRa_driver.h"
 #include "driver/spi_master.h"

 /**
 * @brief  interface the spi bus with the sx1262 and add the device
 * @return status code
 *         - 0 success
 *         - 1 spi init failed
 * @note   none
 */
uint8_t esp32_SPI_bus_init(void);

 /**
 * @brief  free the spi bus from the sx1262 but first remove the device to do so
 * @return status code
 *         - 0 success
 *         - 1 spi deinit failed
 * @note   none
 */
uint8_t esp32_SPI_bus_deinit(void);

 /**
 * @brief  Read and write via SPI one transaction at a time
 * @return status code
 *         - 0 success
 *         - 1 spi read and write asynch failed
 * @note   none
 */
uint8_t esp32_SPI_READ_WRITE_synchronous(void);

 /**
 * @brief  Read and write via SPI multiple times by setting a queue
 * @return status code
 *         - 0 success
 *         - 1 pi read and write asynch failed
 * @note   none
 */
uint8_t esp32_SPI_READ_WRITE_asynchronous(void);





