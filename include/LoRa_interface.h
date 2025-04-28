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
 #include "driver/gpio.h"

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
uint8_t esp32_SPI_WRITE_READ_test(uint8_t *in_buf, uint32_t in_len, uint8_t *out_buf, uint32_t out_len);

 /**
 * @brief  Link the SPI bus the sx1262 and the ESP32 with respect to the ESP32 
 * @return status code
 *         - 1 success
 *         - 0 LoRa chip failed to initialize
 * @note   none
 */

 /**
 * @brief  interface reset gpio init
 * @return status code
 *         - 0 success
 *         - 1 init failed
 * @note   none
 */
uint8_t sx1262_interface_reset_gpio_init(void);

/**
 * @brief  interface reset gpio deinit
 * @return status code
 *         - 0 success
 *         - 1 deinit failed
 * @note   none
 */
uint8_t sx1262_interface_reset_gpio_deinit(void);

/**
 * @brief     interface reset gpio write
 * @param[in] data written data
 * @return    status code
 *            - 0 success
 *            - 1 write failed
 * @note      none
 */
uint8_t sx1262_interface_reset_gpio_write(uint8_t data);

/**
 * @brief  interface busy gpio init
 * @return status code
 *         - 0 success
 *         - 1 init failed
 * @note   none
 */
uint8_t sx1262_interface_busy_gpio_init(void);

/**
 * @brief  interface busy gpio deinit
 * @return status code
 *         - 0 success
 *         - 1 deinit failed
 * @note   none
 */
uint8_t sx1262_interface_busy_gpio_deinit(void);

/**
 * @brief      interface busy gpio read
 * @param[out] *value pointer to a value buffer
 * @return     status code
 *             - 0 success
 *             - 1 read failed
 * @note       none
 */
uint8_t sx1262_interface_busy_gpio_read(uint8_t *value);

/**
 * @brief     interface delay ms
 * @param[in] ms time
 * @note      none
 */
void sx1262_interface_delay_ms(uint32_t ms);

/**
 * @brief     interface print format data
 * @param[in] fmt format data
 * @note      none
 */
void sx1262_interface_debug_print(const char *const fmt, ...);

/**
 * @brief     interface receive callback
 * @param[in] type receive callback type
 * @param[in] *buf pointer to a buffer address
 * @param[in] len buffer length
 * @note      none
 */
void sx1262_interface_receive_callback(uint16_t type, uint8_t *buf, uint16_t len);

/**
 * @}
 */

uint8_t sx1262_device_init(sx1262_handle_t *LoRa_handle);






