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
// sht3x sensor driver REFERENCE: https://github.com/gschorcht/sht3x-esp-idf/tree/master

// TODO: Replace references to write_to_sensor() and read_from_sensor() with
// respective I2C_Write() and I2C_Read() functions.

#include <stddef.h>

#include "../../include/Sensors.h"
#include "driver/i2c_master.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// I2C DEFINES
#define I2C_MASTER_SCL_IO CONFIG_I2C_MASTER_SCL // GPIO for clock. by  |idf.py menuconfig|
#define I2C_MASTER_SDA_IO CONFIG_I2C_MASTER_SDA // GPIO for data line
#define I2C_MASTER_NUM 0						// Which I2C Port we're using.
#define I2C_MASTER_FREQ_HZ 40000				/*!< I2C master clock frequency */
#define I2C_MASTER_TX_BUF_DISABLE 0				/*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE 0				/*!< I2C master doesn't need buffer */
#define I2C_MASTER_TIMEOUT_MS 1000

#define WRITE_BIT I2C_MASTER_WRITE
#define READ_BIT I2C_MASTER_READ
#define ACK_CHECK_EN 0x1
#define ACK_CHECK_DIS 0x0
#define ACK_VAL I2C_MASTER_ACK
#define NACK_VAL I2C_MASTER_NACK

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
static i2c_device_config_t HumidTemp_Cfg = {
	.dev_addr_length = I2C_ADDR_BIT_LEN_7,
	.device_address = SHT3x_ADDR_1,
	.scl_speed_hz = I2C_MASTER_FREQ_HZ,
};


// Sensor device handles
static dev_handle_t Soil_Handle;
static dev_handle_t HumidTemp_Handle;


static const char *TAG = "Sensors";

// SHT3X Variables
sht3x_sensor_t *SHT3X_DataStruct;
sht3x_mode_t SHT3X_Mode = SHT3X_REPEATABILITY;
sht3x_repeat_t SHT3X_Repeat = SHT3X_PERIOD;

static void delay_ms(int ms)
{
	vTaskDelay((ms) / portTICK_PERIOD_MS);
}

SensorsIDs_t Sensors_Init(SensorsIDs_t Sensors)
{
	static uint8_t I2C_InitStatus = 0;
	uint8_t StatusByte;
	SensorsIDs_t ReturnStatus;

	esp_err_t I2C_Result;

	// First step will be to Initialize the I2C Bus.
	// Inititialize I2C Bus, if it hasn't been initialized already.
	if (!I2C_InitStatus)
	{
		ESP_ERROR_CHECK(i2c_new_master_Bus(&i2c_bus_config, &Bus_Handle));
	}

	// If I2C_Init() passes, set I2C_STATUS to 1.
	I2C_InitStatus = 1;

	// Initialize sensors:
	// Each device must be added to master bus, and some devices may require
	// initialization commands (doubtful, though)
	ReturnStatus = 0;
	if (Sensors && SOIL)
	{
		ESP_ERROR_CHECK(i2c_master_bus_add_device(&Bus_Handle, &Soil_Cfg, &Soil_Handle));

	}

	if (Sensors && WIND)
	{
		// Initialization code:
		//	1. Initialize ADC Module
		//  2. Verify connectivity of sensor
		ReturnStatus |= WIND;
	}

	if (Sensors && AIR)
	{
		// Initialization code

		ReturnStatus |= AIR;
	}

	// This sensor requires us to connect the sensor to the bus, and to
	// initialize a data struct by calling sht3x_init_sensor()
	if (Sensors && HUMID_TEMP)
	{
		ESP_ERROR_CHECK(i2c_master_bus_add_device(&Bus_Handle, &HumidTemp_Cfg, &HumidTemp_Handle));

		SHT3X_DataStruct = sht3x_init_sensor(I2C_MASTER_NUM, SHT3x_ADDR_1);
		// Check for error in inititalization.
		if (SHT3X_DataStruct != NULL)
		{
			ReturnStatus |= HUMID_TEMP;
		}
		else
		{
			ESP_LOGW(TAG, "SHT3X Initialization failed");
		}
	}

	return ReturnStatus;
}

esp_err_t Read_SoilMoisture(short *Reading)
{
	esp_err_t I2C_Result;
	uint8_t Write_Buffer[2];

	size_t Read_Buffer_Size = SOIL_MOISTURE_DATA_LENGTH;
	uint8_t Read_Buffer[SOIL_MOISTURE_DATA_LENGTH];

	Write_Buffer[0] = STEMMA_MOISTURE_BASE_REG;
	Write_Buffer[1] = STEMMA_MOISTURE_FUNC_REG

	I2C_Result = ESP_ERROR_CHECK(i2c_master_transmit_receive(&Soil_Handle, Write_Buffer, sizeof(Write_Buffer), Read_Buffer, Read_Buffer_size,8000))

	// Transfer data into variable passed by reference
	*Reading = ((uint16_t)Read_Buffer[0] << 8) | Read_Buffer[1];

	return I2C_Result;
}

esp_err_t Read_SoilTemperature(float *Reading)
{
	esp_err_t I2C_Result;
	unt8_t Write_Buffer[2];

	size_t Read_Buffer_Size = SOIL_TEMP_DATA_LENGTH;
	uint8_t Read_Buffer[SOIL_TEMP_DATA_LENGTH];
	
	Write_Buffer[0] = STEMMA_MOISTURE_BASE_REG;
	Write_Buffer[1] = STEMMA_MOISTURE_FUNC_REG;

	I2C_Result = ESP_ERROR_CHECK(i2c_master_transmit_receive(&HumidTemp_Handle, Write_Buffer, sizeof(Write_Buffer), Read_Buffer, Read_Buffer_Size, 8000));	

	int32_t raw_temp = ((uint32_t)Write_Buffer[0] << 24) | ((uint32_t)Write_Buffer[1] << 16) | ((uint32_t)Write_Buffer[2] << 8) | Write_Buffer[3];
	*Reading = (1.0 / (1UL << 16)) * raw_temp; // normalize value

	return I2C_Result;
}

bool Read_Air_HumidityTemperature(float *Temp_Reading, float *Humid_Reading)
{
	// Start reading
	if (!sht3x_start_measurement(SHT3X_DataStruct, SHT3X_Mode, SHT3X_Repeat))
	{
		return false;
	}

	// Get results of (last) reading
	if (!sht3x_get_results(SHT3X_DataStruct, Temp_Reading, Humid_Reading))
	{
		return false;
	}

	return true;
}
