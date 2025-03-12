/**
 * @file Sensors.c
 * @author Jacob Dennon (jdennon@ucsc.edu)
 * @brief Library for interfacing with sensors.
 * @version 0.1
 * @date 2025-03-08
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#include "../include/Sensors.h"
#include "../include/I2C.h"

SensorsIDs_t Sensors_Init(SenorsIDs_t Sensors) {
    SensorsIDs_t ReturnStatus = 0;

    // Check which sensors are to be initialized
    if(Sensors && SOIL) {
        // initialization code for soil sensor
        
        ReturnStatus |= SOIL; // indicate soil sensor was correctly initialized
    }

    if(Sensors && WIND) {
        // Initialization code

        ReturnStatus |= WIND;
    }


    if(Sensors && AIR) {
        // Initialization code

        ReturnStatus |= AIR;
    }

    if(Sensors && HUMID) {
        // Initialization code

        ReturnStatus |= HUMID;
    }

    return ReturnStatus;
}


SesnorErrors_t Soil_Read(short *Reading) {
    SesnorErrors_t ReturnStatus = 0;

    // I2C HAL CODE

    // data verification code

    return ReturnStatus;
}

/**
 * @brief 
 * 
 * @param i2c_num 
 * @param addr 
 * @param base_reg 
 * @param func_reg 
 * @return esp_err_t 
 */
esp_err_t Soil_Write(i2c_port_t i2c_num, uint8_t addr, uint8_t base_reg, uint8_t func_reg) {
    esp_err_t ret;
    int len = 2;
    uint8_t *moisture_data = (uint8_t *)malloc(len);

    ret = write_to_sensor(i2c_num, STEMMA_SENSOR_ADDR, STEMMA_MOISTURE_BASE_REG, STEMMA_MOISTURE_FUNC_REG);
    if (ret != ESP_OK) 
    {
        ESP_LOGW(TAG, "Write to I2C sensor failed");
        free(moisture_data);
        return ret;
    }

    delay_ms(50);

    ret = read_from_sensor(i2c_num, STEMMA_SENSOR_ADDR, moisture_data, len);
    if (ret == ESP_OK)
    {
        *moisture_value = ((uint16_t)moisture_data[0] << 8) | moisture_data[1];
    }
    else
    {
        ESP_LOGW(TAG, "Read I2C sensor failed");
    }

    free(moisture_data);
    return ret;
}