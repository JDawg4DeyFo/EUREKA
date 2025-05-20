/**
 * @file MonitorTest.c
 * @author Jacob Dennon (jdennon@ucsc.edu)
 * @brief 
 * @version 0.1
 * @date 2025-05-20
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include "../include/INA219.h"

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define I2C_SCL 42
#define I2C_SDA 41
#define I2C_PORT 0

static const char *TAG = "Monitor Testing";

ina219_t Handle;

static void delay_ms(int ms)
{
    vTaskDelay((ms) / portTICK_PERIOD_MS);
}

void app_main(void)
{
	//config
	ina219_init_desc(&Handle, INA219_ADDR_GND_GND, I2C_PORT, I2C_SDA, I2C_SCL);
	// init

	while (1)
	{
		// delay

		// measure
	}

}