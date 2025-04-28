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
#include "Sensors.h"

#define MICROSECOND_CONVERSION 1000000UL

static const char *TAG = "PowerTest.c";

void app_main(void)
{
	ESP_LOGI(TAG, "Welcome to the power consumption test harness!");
	ESP_LOGI(TAG, "Peparing to enter light sleep mode...");

	// Configure the wakeup timer
	const int wakeup_tiem_sec = 60;
	esp_sleep_enable_timer_wakeup(wakeup_time_sec * MICROSECOND_CONVERSION);

	ESP_LOGI(TAG, "Entering light sleep for %d seconds...", wakeup_time_sec);

	// Light sleep start
	esp_light_sleep_start();

	// Program will resume here once timer reaches wakeup time
	ESP_LOGI(TAG, "Wokeup from light sleep!");



	while(1);
}



/*
Tests to conduct:
	1. Light sleep
	2. Deep sleep
	3. Standby
	4. Transmitting
*/