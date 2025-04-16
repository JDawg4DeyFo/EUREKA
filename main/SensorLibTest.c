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
static const char *TAG = "Sesnor-library test";

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static void delay_ms(int ms)
{
    vTaskDelay((ms) / portTICK_PERIOD_MS);
}

void app_main(void)
{
	esp_err_t ret;
	short moisture;
	float temp;
	// float humid;
	int iteration_count = 0;
	
	printf("\nSoil Sensor Test Harness!!!\n");
	printf("SCL: %d, SDA: %d", CONFIG_I2C_MASTER_SCL, CONFIG_I2C_MASTER_SDA);

	SensorsIDs_t sensors = SOIL | HUMID_TEMP;
	printf("Before sensor init");
	SensorsIDs_t initialized = Sensors_Init(sensors);


	

	while (1)
	{
		iteration_count++;
		ret = Read_SoilMoisture(&moisture);

		// ORIGINAL TEST HARNESS:
		// iteration_count++;
		// if (initialized & SOIL)
		// {
		// 	ret = Read_SoilMoisture(&moisture);
		// 	if (ret == ESP_OK)
		// 	{
		// 		ESP_LOGI(TAG, "Soil Moisture: %d", moisture);
		// 	}
		// 	else
		// 	{
		// 		ESP_LOGW(TAG, "Failed to read soil moisture");
		// 	}

		// 	ret = Read_SoilTemperature(&temp);
		// 	if (ret == ESP_OK)
		// 	{
		// 		ESP_LOGI(TAG, "Soil Temperature: %.2f", temp);
		// 	}
		// 	else
		// 	{
		// 		ESP_LOGW(TAG, "Failed to read soil temperature");
		// 	}
		// }
		// else
		// {
		// 	ESP_LOGW(TAG, "soil sensor not initialized");
		// }

		// if (initialized & HUMID_TEMP)
		// {
		// 	ret = Read_Air_HumidityTemperature(&temp, &humid);
		// 	if (ret)
		// 	{
		// 		ESP_LOGI(TAG, "temperature: %f, humidity: %f", temp, humid);
		// 	}
		// 	else
		// 	{
		// 		ESP_LOGW(TAG, "Failed to read humidity and temperature");
		// 	}
		// }
		// else
		// {
		// 	ESP_LOGW(TAG, "Humidity temp sensor not initialized");
		// }

		delay_ms(500);
		ESP_LOGI(TAG, "While loop iteration #: %d\n", iteration_count);
		ESP_LOGI(TAG, "Moisture reading: %d", moisture);
		fflush(stdout);

	}

}