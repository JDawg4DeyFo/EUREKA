/*
 * File: I2C.h
 * Author: Jacob Dennon
 *
 * Version: 0.1
 * Last Edited: March 6, 2025
 * Created on March 6, 2025
 *
 */

#include <stdio.h>
#include "esp_log.h"
#include "driver/i2c.h"

/*******************************************************************************
 * PUBLIC #DEFINES                                                            *
 ******************************************************************************/
#define I2C_MASTER_SCL_IO           CONFIG_I2C_MASTER_SCL      // GPIO for clock. by  |idf.py menuconfig|
#define I2C_MASTER_SDA_IO           CONFIG_I2C_MASTER_SDA      // GPIO for data line
#define I2C_MASTER_NUM              0                          // Which I2C Port we're using.
#define I2C_MASTER_FREQ_HZ          400000                     /*!< I2C master clock frequency */
#define I2C_MASTER_TX_BUF_DISABLE   0                          /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE   0                          /*!< I2C master doesn't need buffer */
#define I2C_MASTER_TIMEOUT_MS       1000

/*******************************************************************************
 * PUBLIC DATATYPES
 ******************************************************************************/


/*******************************************************************************
 * PUBLIC FUNCTIONS                                                           *
 ******************************************************************************/
/**
 * @brief Initialize I2C module
 * 
 * @return ESP Error type
 */
esp_err_t I2C_Init(void);

/**
 * @brief Read from I2C_Register
 * 
 * @param Device_Address I2C address of the device to be read from.
 * @param Register_Address Address of the device's register you wish to read from.
 * @param data Pointer to variable where data will be stored.
 * @param len Number of bytes to read.
 * @return ESP Error type
 * 
 */
esp_err_t I2C_Read(uint8_t Device_Address, uint8_t Register_Address, uint8_t *data, size_t len);


/**
 * @brief Write to an I2C Device
 * 
 * @param Device_Address I2C address of the device to be written to
 * @param Register_Address Address of the device to be written to
 * @param data data to be written
 * @return ESP Error type 
 */
esp_err_t I2C_Write(uint8_t Device_Address, uint8_t Register_Address, uint8_t data);


// TO design and code: will scan I2C bus for connected devices
esp_err_t I2C_Scan();
