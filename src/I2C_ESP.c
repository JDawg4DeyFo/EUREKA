/*
 * File: I2C_ESP.c
 * Author: Jacob Dennon
 *
 * Version: 0.1
 * Last Edited: March 6, 2025
 * Created on March 6, 2025
 *
 */

#include "../include/I2C_ESP.h"

esp_err_t I2C_Read(uint8_t Device_Address, uint8_t Register_Address, uint8_t *data, size_t len) {
    // NEED TO INVESTIGATE HOW THIS FUNCTION WORKS!!
    // It's a default function from ESP's HAL, but there's a caveat:
    // Some sensors will respond to a read request in different ways!
    return i2c_master_write_read_device(I2C_MASTER_NUM, Device_Address, &Register_Address, 1, data, len, I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
}


esp_err_t I2C_Write(uint8_t Device_Address, uint8_t Register_Address, uint8_t data) {
    int ReturnValue;
    uint8_t write_buf[2] = {Register_Address, data}; // investigate how this is used

    // NEED TO INVESTIGATE HOW THIS FUNCTION WORKS!!
    // It'll probably work fine, but I need to know how the HAL functions
    // operate in order to proceed with confidence.
    ReturnValue = i2c_master_write_to_device(I2C_MASTER_NUM, Device_Address, write_buf, sizeof(write_buf), I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
}

esp_err_t I2C_Init(void) {
    int i2c_master_port = I2C_MASTER_NUM;

    // Configureation typedef passed to HAL function
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,    // set in menuconfig
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ,
    };

    i2c_param_config(i2c_master_port, &config);

    // NEED TO INVESTIGATE!!!
    // so many gaps of knowledge to fill in.
    return i2c_driver_INSTALL(i2c_master_port, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);
}


// Unfinished test harness
#ifdef ESP_I2C_TEST

int main(void) {
    while(1);
}

#endif