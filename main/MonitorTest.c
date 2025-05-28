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

#include "../esp-lib/components/ina219/ina219.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <string.h>

#define I2C_SCL 42
#define I2C_SDA 41
#define I2C_PORT 0

#define MAX_CURRENT 0.006
#define SHUNT_RESISTANCE 47

static const char *TAG = "Monitor Testing";

static void delay_ms(int ms)
{
    vTaskDelay((ms) / portTICK_PERIOD_MS);
}

void app_main(void)
{
	float BusVoltage, ShuntVoltage, Current, Power;

	BusVoltage = 0;
	ShuntVoltage = 0;
	Current = 0;
	Power = 0;

	ina219_t Handle;
	memset(&Handle, 0, sizeof(ina219_t));

	//config
	ina219_init_desc(&Handle, INA219_ADDR_GND_GND, I2C_PORT, I2C_SDA, I2C_SCL);

	// i2cdev_init();
	
	// init
	ina219_init(&Handle);
	ina219_configure(&Handle, INA219_BUS_RANGE_32V, INA219_GAIN_0_125, INA219_RES_12BIT_1S, INA219_RES_12BIT_1S, INA219_MODE_CONT_SHUNT_BUS);
	ina219_calibrate(&Handle, SHUNT_RESISTANCE);


	while (1)
	{
		// delay
		delay_ms(1000);

		// measure
		ina219_get_current(&Handle, &Current);
		ina219_get_bus_voltage(&Handle, &BusVoltage);
		ina219_get_shunt_voltage(&Handle, &ShuntVoltage);
		ina219_get_current(&Handle, &Current);
		ina219_get_power(&Handle, &Power);

		// print out power
		ESP_LOGI(TAG, "Measurements....\n\tBus Voltage: %f\n\tShunt Voltage: %f\n\tCurrent:%f\n\tPower:%f", BusVoltage, ShuntVoltage, Current, Power);
	}

}