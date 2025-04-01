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


// REFERENCE: https://learn.adafruit.com/adafruit-seesaw-atsamd09-breakout/reading-and-writing-data

// TODO: Replace references to write_to_sensor() and read_from_sensor() with 
// respective I2C_Write() and I2C_Read() functions.

#include "../include/Sensors.h"
#include "../include/I2C.h"

static const char *TAG = "Sensors";

SensorsIDs_t Sensors_Init(SenorsIDs_t Sensors) {
    esp_err_t I2C_Result;
    uint8_t StatusByte;

    SensorsIDs_t ReturnStatus = 0;

    if(Sensors && SOIL) {
        // Poll status register for device ID to determine if sensor and i2c bus
        // have been properly initialized

        // From reference: A register read is accomplished by first sending the 
        // standard I2C write header, followed by the two register bytes 
        // corresponding to the data to be read. Allow a short delay, and then 
        // send a standard I2C read header (with the R/W bit set to 1) to read 
        // the data.

        I2C_Result = I2C_Write(I2C_MASTER_NUM, STEMMA_SENSOR_ADDR, STEMMA_STATUS_BASE_REG, STEMMA_STATUS_HWID_REG);
        IF (I2C_Result != ESP_OK) {
            ESP_LOGW(TAG, "Write to I2C sensor failed");
        }

        delay_ms(50); // VERIFY that 50 ms is proper delay for this sensor

        // INVESTIGATE: Might need to replace this read with a standard I2C read
        // header...
        I2C_Result = I2C_Read(I2C_MASTER_NUM, STEMMA_SENSOR_ADDR, STEMMA_STATUS_BASE_REG, STEMMA_STATUS_HWID_REG);
        
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


#ifdef SENSOR_TEST
void app_main(void) {
    esp_err_t ret;
    uint16_t moisture;
    float temp;

    ret = I2C_Init();
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "I2C initialization failed");
        return;
    }

    SensorsIDs_t sensors = SOIL | WIND;
    SensorsIDs_t initialized = Sensors_Init(sensors);

    if (initialized & SOIL)
    {
        ret = Read_SoilMoisture(&moisture);
        if (ret == ESP_OK)
        {
            ESP_LOGI(TAG, "Soil Moisture: %d", moisture);
        }
        else
        {
            ESP_LOGW(TAG, "Failed to read soil moisture");
        }
    }

    if (initialized & WIND)
    {
        ret = Read_SoilTemperature(&temp);
        if (ret == ESP_OK)
        {
            ESP_LOGI(TAG, "Soil Temperature: %.2f", temp);
        }
        else
        {
            ESP_LOGW(TAG, "Failed to read soil temperature");
        }
    }
}   
#endif // SENSOR_TEST