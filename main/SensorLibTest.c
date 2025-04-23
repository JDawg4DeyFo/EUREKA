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
static const char *TAG = "Sensor-library test";

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static void delay_ms(int ms)
{
    vTaskDelay((ms) / portTICK_PERIOD_MS);
}

void app_main(void)
{
	short soil_moisture;
	float soil_temp;
	float temp;
	float humid;
	float WindDirection;
	float WindSpeed;
	int iteration_count = 0;
	
	printf("\nSoil Sensor Test Harness!!!\n");
	printf("SCL: %d, SDA: %d", CONFIG_I2C_MASTER_SCL, CONFIG_I2C_MASTER_SDA);

	SensorsIDs_t sensors = SOIL | SHT30 | WINDVANE | ANEMOMETER;
	Sensors_Init(sensors);

	printf("Sensor Inititalization passed!\n");

	

	while (1)
	{
		iteration_count++;
		ESP_LOGI(TAG, "\n Testing Sensor readings. Iteration number %d\n", iteration_count);

		ESP_LOGI(TAG, "Testing soil moisture Reading.\n");
		if(Read_SoilMoisture(&soil_moisture) != ESP_OK) {
			ESP_LOGW(TAG, "\tSoil Moisture Reading fail.\n");
		}
		else {
			ESP_LOGI(TAG, "\tSoil Moisture Reading: %d", soil_moisture);
		}

		ESP_LOGI(TAG, "Testing soil temperature Reading.\n");
		if(Read_SoilTemperature(&soil_temp) != ESP_OK) {
			ESP_LOGW(TAG, "\tSoil Temperature Reading fail.\n");
		}
		else {
			ESP_LOGI(TAG, "\tSoil Temperature Reading: %f\n", soil_temp);
		}
		
		ESP_LOGI(TAG, "Testing SHT30 humidity temperature reading.\n");
		if(Read_SHT30_HumidityTemperature(&temp, &humid) != true) {
			ESP_LOGW(TAG, "\tSHT30 reading fail.\n");
		}
		else {
			ESP_LOGI(TAG, "\tSuccess! temperature: %f humidity: %f\n", temp, humid);
		}

		ESP_LOGI(TAG, "Testing wind direction reading.\n");
		ESP_LOGI(TAG, "Note: no fail condition for this test.\n");
		WindDirection = Get_Wind_Direction();
		ESP_LOGI(TAG, "\t Wind direction: %f\n", WindDirection);

		ESP_LOGI(TAG, "Testing wind speed reading.\n");
		ESP_LOGI(TAG, "Note: no fail condition for this test.\n");
		WindSpeed = Get_Wind_Speed();
		ESP_LOGI(TAG, "\t Wind speed: %f\n", WindSpeed);

		delay_ms(500);

		fflush(stdout);

	}

}