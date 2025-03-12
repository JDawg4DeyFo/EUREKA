/*
 * File: I2C_ESP.c
 * Author: Jacob Dennon
 *
 * Version: 0.1
 * Last Edited: March 6, 2025
 * Created on March 6, 2025
 *
 */

#include "../include/I2C.h"

esp_err_t I2C_Read(uint8_t Device_Address, uint8_t Base_Register, uint8_t, Function_Register, uint8_t *data, size_t len) {
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, Device_Address << 1 | READ_BIT, ACK_CHECK_EN);
    i2c_master_read(cmd, data, len - 1, ACK_VAL);
    i2c_master_read_byte(cmd, data + len - 1, NACK_VAL);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
    return ret;
}


esp_err_t I2C_Write(uint8_t Device_Address, uint8_t Base_Register, uint8_t Function_Register, uint8_t data) {
    esp_err_t ReturnValue;
    uint8_t write_buf[2] = {Register_Address, data}; // investigate how this is used

    // Process is outlined 
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, Device_Address << 1 | WRITE_BIT, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, Base_Register, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, Function_Register, ACK_CHECK_EN);
    i2c_master_stop(cmd);
    ReturnValue = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
    
    return ReturnValue;
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

    i2c_param_config(i2c_master_port, &conf);

    return i2c_driver_INSTALL(i2c_master_port, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);
}


// Unfinished test harness
#ifdef ESP_I2C_TEST

int main(void) {
    while(1);
}

#endif