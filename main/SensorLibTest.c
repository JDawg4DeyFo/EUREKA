/**
 * @file SensorLibTest.c
 * @author Jacob Dennon (jdennon@ucsc.edu)
 * @brief 
 * @version 0.1
 * @date 2025-04-08
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include "../include/Sensors.h"

#include "esp_log.h"
static const char *TAG = "i2c-simple-example";

void app_main(void)
{
	esp_err_t ret;
	short moisture;
	float temp;
	float humid;

	ret = I2C_Init();
	if (ret != ESP_OK)
	{
		ESP_LOGE(TAG, "I2C initialization failed");
		return;
	}

	SensorsIDs_t sensors = SOIL | HUMID_TEMP;
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

		ret = Read_SoilTemperature(&temp);
		if (ret == ESP_OK)
		{
			ESP_LOGI(TAG, "Soil Temperature: %.2f", temp);
		}
		else
		{
			ESP_LOGW(TAG, "Failed to read soil temperature");
		}
	} else 
	{
		ESP_LOGW(TAG, "soil sensor not initialized");
	}

	if (initialized & HUMID_TEMP)
	{
		ret = Read_Air_HumidityTemperature(&temp, &humid);
		if (ret)
		{
			ESP_LOGI(TAG, "temperature: %f, humidity: %f", temp, humid);
		}
		else
		{
			ESP_LOGW(TAG, "Failed to read humidity and temperature");
		}
	} else {
		ESP_LOGW(TAG, "Humidity temp sensor not initialized");
	}
}