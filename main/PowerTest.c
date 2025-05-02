/**
 * @file PowerTest.c
 * @author Jacob Dennon (jdennon@ucsc.edu)
 * @brief Testing different power states
 * @version 0.1
 * @date 2025-04-27
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include "esp_log.h"
#include "esp_sleep.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"
#include "esp_task_wdt.h"

#include "../include/Sensors.h"
#include "../include/LoRa_driver.h"
#include "../include/LoRa_main.h"


#define MICROSECOND_CONVERSION 1000000

static const char *TAG = "PowerTest.c";

static void delay_ms(int ms)
{
    vTaskDelay((ms) / portTICK_PERIOD_MS);
}

static sx1262_handle_t LORA_Handle;

void app_main(void)
{
	const int wakeup_time_sec = 10;
	const int standby_time_sec = 30;
	int TX_time_sec = 30;
	const int RX_time_sec = 30;

	int64_t start_time;
	int iteration_count = 0;
	int foo = 0;
	float temp, humid, WindDirection, WindSpeed, soil_temp;
	short soil_moisture;
	uint8_t ret;

	ESP_LOGI(TAG, "Welcome to the power consumption test harness!");

	// Disable watchdog
	esp_task_wdt_deinit();

// Standby state test
/******************************************************************************/
	ESP_LOGI(TAG, "Preparing to test Standby mode...");

	// Initialize sensor module
	Sensors_Init(ALL_SENSORS);

	// Get start time
	start_time = esp_timer_get_time();

	// Main loop
	ESP_LOGI(TAG, "Entering Standby mode for %d seconds...", standby_time_sec);
	while ((esp_timer_get_time() - start_time) < (standby_time_sec * MICROSECOND_CONVERSION)) {
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

		for(int i = 0; i < 90000000; i++) {
			foo++;
		}
	}

// TX state test
/******************************************************************************/
	ESP_LOGI(TAG, "Preparing to enter TX state...");
	if (!sx1262_lora_begin(&LORA_Handle)) {
		ESP_LOGI(TAG, "LoRa Initialized correctly");
		ESP_LOGI(TAG, "Beginning continous wave output...");

		// Set LoRa to send mode
		ret = sx1262_lora_set_send_mode(&LORA_Handle);
		if (ret) {
			// If fail, set hold time to 0
			ESP_LOGE(TAG, "Error setting send mode!");
			TX_time_sec = 0;
		} else {
			ESP_LOGI(TAG, "Testing TX state for %d seconds", TX_time_sec);
		}

		// Set continous wave output
		ret = sx1262_set_tx_continuous_wave(&LORA_Handle);
		if (ret) {
			// If fail, set hold time to 0
			ESP_LOGE(TAG, "Error setting continous wave! Code: %d", ret);
			TX_time_sec = 0;
		} else {
			ESP_LOGI(TAG, "Testing TX state for %d seconds", TX_time_sec);
		}

		// Hold continous wave output
		start_time = esp_timer_get_time();
		while ((esp_timer_get_time() - start_time) < (TX_time_sec * MICROSECOND_CONVERSION));


	} else {
		ESP_LOGE(TAG, "Error initializing LoRa");
	}

// RX state test
/******************************************************************************/
	ESP_LOGI(TAG, "Preparing to enter RX state...");
	if (sx1262_lora_set_continuous_receive_mode(&LORA_Handle)) {
		ESP_LOGE(TAG, "Error enterring RX state!");
	} else {
		ESP_LOGI(TAG, "Testing RX state for %d seconds", RX_time_sec);
		start_time = esp_timer_get_time();
		// hold program, resetting watchdog timer
		while ((esp_timer_get_time() - start_time) < (RX_time_sec * MICROSECOND_CONVERSION));
	}


// NOTE: Only 1 sleep state can be tested, and they may be equivalent...
//		Execution context is not saved on wakeup.

// Light sleep state test
/******************************************************************************/
	ESP_LOGI(TAG, "Peparing to enter light sleep mode...");

	// Configure the wakeup timer
	esp_sleep_enable_timer_wakeup(wakeup_time_sec * MICROSECOND_CONVERSION);

	Deinitialize_Sensors();

	// Light sleep start
	ESP_LOGI(TAG, "Entering light sleep for %d seconds...", wakeup_time_sec);
	esp_deep_sleep_start(); // deep sleep mode but LoRa wakeup is enabled

	// Program will resume here once timer reaches wakeup time
	ESP_LOGI(TAG, "Wokeup from light sleep!");

// Deep sleep state test
/******************************************************************************/
	ESP_LOGI(TAG, "Preparing to enter deep sleep mode...");

	// Configure the wakeup timer
	esp_sleep_enable_timer_wakeup(wakeup_time_sec * MICROSECOND_CONVERSION);

	ESP_LOGI(TAG, "Entering deep sleep for %d seconds...", wakeup_time_sec);

	// Light sleep start
	esp_deep_sleep_start(); // deep sleep mode but LoRa wakeup is enabled

	// Program will resume here once timer reaches wakeup time
	ESP_LOGI(TAG, "Wokeup from deep sleep!");

	while(1);
}