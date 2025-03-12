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


esp_err_t Read_SoilMoisture(uint16_t *Reading) {
    esp_err_t ret;
    int len = 2;
    uint8_t *moisture_data = (uint8_t *)malloc(len);

    ret = write_to_sensor(I2C_MASTER_NUM, STEMMA_SENSOR_ADDR, STEMMA_MOISTURE_BASE_REG, STEMMA_MOISTURE_FUNC_REG);
    if (ret != ESP_OK) 
    {
        ESP_LOGW(TAG, "Write to I2C sensor failed");
        free(moisture_data);
        return ret;
    }

    delay_ms(50);

    ret = read_from_sensor(I2C_MASTER_NUM, STEMMA_SENSOR_ADDR, moisture_data, len);
    if (ret == ESP_OK)
    {
        *Reading = ((uint16_t)moisture_data[0] << 8) | moisture_data[1];
    }
    else
    {
        ESP_LOGW(TAG, "Read I2C sensor failed");
    }

    free(moisture_data);
    return ret;
}

esp_err_t Read_SoilTemperature(float *Reading) {
    esp_err_t ret;
    int len = 4;
    uint8_t *temp_data = (uint8_t *)malloc(len);

    ret = write_to_sensor(I2C_MASTER_NUM, STEMMA_SENSOR_ADDR, STEMMA_TEMP_BASE_REG, STEMMA_TEMP_FUNC_REG);
    if (ret != ESP_OK)
    {
        ESP_LOGW(TAG, "Write to I2C sensor failed");
        free(temp_data);
        return ret;
    }

    delay_ms(50);

    ret = read_from_sensor(I2C_MASTER_NUM, STEMMA_SENSOR_ADDR, temp_data, len);
    if (ret == ESP_OK)
    {
        int32_t raw_temp = ((uint32_t)temp_data[0] << 24) | ((uint32_t)temp_data[1] << 16) | ((uint32_t)temp_data[2] << 8) | temp_data[3];
        *Reading = (1.0 / (1UL << 16)) * raw_temp;
    }
    else
    {
        ESP_LOGW(TAG, "Read I2C sensor failed");
    }

    free(temp_data);
    return ret;
}