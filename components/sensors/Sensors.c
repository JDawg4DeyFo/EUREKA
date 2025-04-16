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

// soil sensor REFERENCE: https://learn.adafruit.com/adafruit-seesaw-atsamd09-breakout/reading-and-writing-data
// sht3x sensor REFERENCE: https://sensirion.com/media/documents/213E6A3B/63A5A569/Datasheet_SHT3x_DIS.pdf
// sht3x sensor driver REFERENCE: https://github.com/ma-lwa-re/esp32-sht3x/blob/master/

// TODO: Replace references to write_to_sensor() and read_from_sensor() with
// respective I2C_Write() and I2C_Read() functions.

#include <stddef.h>

#include "../../include/Sensors.h"
#include "driver/i2c_master.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

// standard delay :P
static void delay_ms(int ms)
{
    vTaskDelay((ms) / portTICK_PERIOD_MS);
}


// Master i2c bus configuration
static i2c_master_bus_config_t i2c_bus_config = {
	.clk_source = I2C_CLK_SRC_DEFAULT,
	.i2c_port = I2C_MASTER_NUM,
	.scl_io_num = I2C_MASTER_SCL_IO,
	.sda_io_num = I2C_MASTER_SDA_IO,
	.glitch_ignore_cnt = 7,
};

// i2c bus handle
static i2c_master_bus_handle_t Bus_Handle;

// Sensor device cfgs
static i2c_device_config_t Soil_Cfg = {
	.dev_addr_length = I2C_ADDR_BIT_LEN_7,
	.device_address = STEMMA_SENSOR_ADDR,
	.scl_speed_hz = I2C_MASTER_FREQ_HZ,
};
static i2c_device_config_t SHT30_Cfg = {
	.dev_addr_length = I2C_ADDR_BIT_LEN_7,
	.device_address = SHT3X_SENSOR_ADDR,
	.scl_speed_hz = I2C_MASTER_FREQ_HZ,
};


// Sensor device handles
static i2c_master_dev_handle_t Soil_Handle;
static i2c_master_dev_handle_t SHT30_Handle;


static const char *TAG = "Sensors";

// static void delay_ms(int ms)
// {
// 	vTaskDelay((ms) / portTICK_PERIOD_MS);
// }

SensorsIDs_t Sensors_Init(SensorsIDs_t Sensors)
{
	static uint8_t I2C_InitStatus = 0;
	SensorsIDs_t ReturnStatus;

	// First step will be to Initialize the I2C Bus.
	// Inititialize I2C Bus, if it hasn't been initialized already.
	if (!I2C_InitStatus)
	{
		ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_bus_config, &Bus_Handle));
	}
	// If I2C_Init() passes, set I2C_STATUS to 1.
	I2C_InitStatus = 1;

	// Initialize sensors:
	// Each device must be added to master bus, and some devices may require
	// initialization commands (doubtful, though)
	ReturnStatus = 0;
	if (Sensors && SOIL)
	{
		ESP_ERROR_CHECK(i2c_master_bus_add_device(Bus_Handle, &Soil_Cfg, &Soil_Handle));
		
		ReturnStatus |= SOIL;
	}

	if (Sensors && WINDVANE)
	{
		// Initialization code:
		//	1. Initialize ADC Module
		//  2. Verify connectivity of sensor
		ReturnStatus |= WINDVANE;
	}

	if (Sensors && ANEMOMETER)
	{
		// Initialization code
		// 1. Initialize input compare
		ReturnStatus |= ANEMOMETER;
	}

	// Sensor 2: SHT30
	if (Sensors && SHT30)
	{
		ESP_ERROR_CHECK(i2c_master_bus_add_device(Bus_Handle, &SHT30_Cfg, &SHT30_Handle));
		
		ReturnStatus |= SHT30;
	}

	return ReturnStatus;
}

esp_err_t Read_SoilMoisture(short *Reading)
{
	esp_err_t I2C_Result = ESP_OK;
	uint8_t Write_Buffer[2];

	size_t Read_Buffer_Size = SOIL_MOISTURE_DATA_LENGTH;
	uint8_t Read_Buffer[SOIL_MOISTURE_DATA_LENGTH];

	Write_Buffer[0] = STEMMA_MOISTURE_BASE_REG;
	Write_Buffer[1] = STEMMA_MOISTURE_FUNC_REG;

	ESP_ERROR_CHECK_WITHOUT_ABORT(i2c_master_transmit(Soil_Handle, Write_Buffer, sizeof(Write_Buffer), 1000));

	delay_ms(50);

	ESP_ERROR_CHECK_WITHOUT_ABORT(i2c_master_receive(Soil_Handle, Read_Buffer, Read_Buffer_Size, 1000));

	// Transfer data into variable passed by reference
	*Reading = ((uint16_t)Read_Buffer[0] << 8) | Read_Buffer[1];

	return I2C_Result;
}

esp_err_t Read_SoilTemperature(float *Reading)
{
	esp_err_t I2C_Result = ESP_OK;
	uint8_t Write_Buffer[2];

	size_t Read_Buffer_Size = SOIL_TEMP_DATA_LENGTH;
	uint8_t Read_Buffer[SOIL_TEMP_DATA_LENGTH];
	
	Write_Buffer[0] = STEMMA_MOISTURE_BASE_REG;
	Write_Buffer[1] = STEMMA_MOISTURE_FUNC_REG;

	ESP_LOGI(TAG, "Made it to line 156");

	ESP_ERROR_CHECK_WITHOUT_ABORT(i2c_master_transmit(Soil_Handle, Write_Buffer, sizeof(Write_Buffer), 1000));	

	delay_ms(50);

	ESP_ERROR_CHECK_WITHOUT_ABORT(i2c_master_receive(Soil_Handle, Read_Buffer, Read_Buffer_Size, 1000));

	int32_t raw_temp = ((uint32_t)Read_Buffer[0] << 24) | ((uint32_t)Read_Buffer[1] << 16) | ((uint32_t)Read_Buffer[2] << 8) | Read_Buffer[3];
	*Reading = (1.0 / (1UL << 16)) * raw_temp; // normalize value

	return I2C_Result;
}

bool Read_SHT30_HumidityTemperature(float *Temp_Reading, float *Humid_Reading)
{
	sht3x_sensors_values_t Readings;

	if (sht3x_read_measurement(&Readings, SHT30_Handle) != ESP_OK) {
		return false;
	}

	*Temp_Reading = Readings.temperature;
	*Humid_Reading = Readings.humidity;

	return true;
}
