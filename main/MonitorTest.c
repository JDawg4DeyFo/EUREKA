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

#define MAX_CURRENT 0.006
#define SHUNT_RESISTANCE 50

static const char *TAG = "Monitor Testing";

ina219_t Handle;

static void delay_ms(int ms)
{
    vTaskDelay((ms) / portTICK_PERIOD_MS);
}

void app_main(void)
{
	float BusVoltage, ShuntVoltage, Current, Power;

	//config
	ina219_init_desc(&Handle, INA219_ADDR_GND_GND, I2C_PORT, I2C_SDA, I2C_SCL);
	
	// init
	ina219_init(&Handle);
	ina219_configure(&Handle, INA219_BUS_RANGE_32V, INA219_GAIN_0_125, 12INA219_RES_12BIT_1S, 12INA219_RES_12BIT_1S, INA219_MODE_CONT_SHUNT_BUS);
	ina219_calibrate(&Handle, MAX_CURRENT, SHUNT_RESISTANCE);


	while (1)
	{
		// delay
		delay_ms(100);

		// measure
		ina219_get_current(&Handle, &Current);
		ina219_get_bus_voltage(&Hanlde, &BusVoltage);
		ina219_get_shunt_voltage(&Handle, &ShuntVoltage);
		ina219_get_current(&Handle, &Current);
		ina219_get_power(&Handle, &Power);

		// print out power
		ESP_LOGI("Measurements....\n\tBus Voltage: %f\n\tShunt Voltage: %f\n\tCurrent:%f\n\tPower:%f", BusVoltage, ShuntVoltage, Current, Power);
	}

}