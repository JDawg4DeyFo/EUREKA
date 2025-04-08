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

void app_main(void)
{
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