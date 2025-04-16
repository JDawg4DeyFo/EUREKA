/*
 * File: I2C.h
 * Author: Jacob Dennon
 *
 * Version: 2.0
 * Last Edited: April 14, 2025
 * Created on March 6, 2025
 *
 */
// Common I2C definitions held by driver and sensor library


#include "driver/i2c_master.h"


/*******************************************************************************
 * PUBLIC #DEFINES                                                            *
 ******************************************************************************/
// I2C DEFINES
#define I2C_MASTER_SCL_IO CONFIG_I2C_MASTER_SCL // GPIO for clock. by  |idf.py menuconfig|
#define I2C_MASTER_SDA_IO CONFIG_I2C_MASTER_SDA // GPIO for data line
#define I2C_MASTER_NUM 0						// Which I2C Port we're using.
#define I2C_MASTER_FREQ_HZ 100000				/*!< I2C master clock frequency */
#define I2C_MASTER_TX_BUF_DISABLE 0				/*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE 0				/*!< I2C master doesn't need buffer */
#define I2C_MASTER_TIMEOUT_MS 1000

#define WRITE_BIT I2C_MASTER_WRITE
#define READ_BIT I2C_MASTER_READ
#define ACK_CHECK_EN 0x1
#define ACK_CHECK_DIS 0x0
#define ACK_VAL I2C_MASTER_ACK
#define NACK_VAL I2C_MASTER_NACK

// From sh3x driver:
#define I2C_MASTER_SDA              (CONFIG_I2C_MASTER_SCL)
#define I2C_MASTER_SCL              (CONFIG_I2C_MASTER_SDA)
// #define I2C_MASTER_RX_BUF_DISABLE   (0)
// #define I2C_MASTER_TX_BUF_DISABLE   (0)
// #define I2C_MASTER_FREQ_HZ          (40000)
// #define I2C_MASTER_TIMEOUT_MS       (1000)
// #define I2C_MASTER_NUM              (0)
// #define I2C_ACK_CHECK_DIS           (0x00)
// #define I2C_ACK_CHECK_EN            (0x01)
#define I2C_ACK_VAL                 (I2C_MASTER_ACK)
#define I2C_NACK_VAL                (I2C_MASTER_NACK)