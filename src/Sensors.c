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
// sht3x sensor driver REFERENCE: 


// TODO: Replace references to write_to_sensor() and read_from_sensor() with
// respective I2C_Write() and I2C_Read() functions.

#include "../include/Sensors.h"

static const char *TAG = "Sensors";

// SHT3X Variables
sht3x_sensor_t* SHT3X_DataStruct;
sht3x_mode_t SHT3X_Mode = SHT3X_REPEATABILITY;
sht3x_repeat_t SHT3X_Repeat = SHT3X_PERIOD;

SensorsIDs_t Sensors_Init(SenorsIDs_t Sensors)
{
	static uint8_t I2C_InitStatus = 0;
	uint8_t StatusByte;
	SensorsIDs_t ReturnStatus;

	esp_err_t I2C_Result;

	// First step will be to Initialize the I2C Bus.
	// Inititialize I2C Bus, if it hasn't been initialized already.
	if (!I2C_InitStatus)
	{
		if (I2C_Init() != ESP_OK)
		{
			return 0;
		}
	}

	// If I2C_Init() passes, set I2C_STATUS to 1.
	I2C_InitStatus = 1;

	// 2. Intitialize specified sensors. For those that don't require specific
	//    initizilization instructions, just check device connectivity.
	ReturnStatus = 0;
	if (Sensors && SOIL)
	{
		// Poll status register for device ID to determine if sensor and i2c bus
		// have been properly initialized

		// From reference: A register read is accomplished by first sending the
		// standard I2C write header, followed by the two register bytes
		// corresponding to the data to be read. Allow a short delay, and then
		// send a standard I2C read header (with the R/W bit set to 1) to read
		// the data.

		// Create command link, requesting a read from status register.
		i2c_cmd_handle_t cmd = i2c_cmd_link_create();
		i2c_master_start(cmd);
		i2c_master_write_byte(cmd, STEMMA_SENSOR_ADDR << 1 | WRITE_BIT, ACK_CHECK_EN);
		i2c_master_write_byte(cmd, STEMMA_STATUS_BASE_REG, ACK_CHECK_EN);
		i2c_master_write_byte(cmd, STEMMA_STATUS_HWID_REG, ACK_CHECK_EN);
		i2c_master_stop(cmd);
		I2C_Result = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS)
		i2c_cmd_link_delete(cmd);

		//  Continue with read operation if no problem with control bytes
		if (I2C_Result == ESP_OK) {
			delay_ms(50); // VERIFY that 50 ms is proper delay for this sensor

			i2c_cmd_handle_t cmd = i2c_cmd_link_create();
			i2c_master_start(cmd);
			i2c_master_write_byte(cmd, STEMMA_SENSOR_ADDR << 1 | READ_BIT, ACK_CHECK_EN);
			i2c_master_read_byte(cmd, &StatusByte, NACK_VAL);
			i2c_master_stop(cmd);
			I2C_Result = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
			i2c_cmd_link_delete(cmd);

			// Check data was read and that I2C operation concluded succesfully.
			if ((StatusByte != 0) && (I2C_Result == ESP_OK)) {
				ReturnStatus |= SOIL; // indicate soil sensor was correctly initialized
			} else {
				// error message if read operation failed
				ESP_LOGW(TAG, "Status read for soil sensor failed");
			}
		} else {
			// error message if control bytes failed
			ESP_LOGW(TAG, "Status read request for soil sensor failed");
		}
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

	if (Sensors && HUMID_TEMP)
	{
		// VERIFY: that i2c_master_num is equivalent to i2c bus number
		SoilSensor_DataStruct = sht3x_init_sensor(I2C_MASTER_NUM, SHT3x_ADDR_1);

		// Check for error in inititalization.
		if (SoilSensor_DataStruct != NULL) {
			ReturnStatus |= HUMID_TEMP
		} else {
			ESP_LOGW(TAG, "SHT3X Initialization failed");
		}
	}

	return ReturnStatus;
}

esp_err_t Read_SoilMoisture(uint16_t *Reading)
{
	esp_err_t I2C_Result;
	int len = SOIL_MOISTURE_DATA_LENGTH;
	uint8_t moisture[SOIL_MOISTURE_DATA_LENGTH];

	// Send out control bytes, indicating soil moisture read request
	i2c_cmd_handle_t cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, STEMMA_SENSOR_ADDR << 1 | WRITE_BIT, ACK_CHECK_EN);
	i2c_master_write_byte(cmd, STEMMA_MOISTURE_BASE_REG, ACK_CHECK_EN);
	i2c_master_write_byte(cmd, STEMMA_MOISTURE_FUNC_REG, ACK_CHECK_EN);
	i2c_master_stop(cmd);
	I2C_Result = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS)
	i2c_cmd_link_delete(cmd);

	// Check that request passed through
	if (I2C_Result != ESP_OK) {
		ESP_LOGW(TAG, "Moisture Request failed");
        return I2C_Result;
	}

	// short delay as requested by data sheet
	delay_ms(50);

	// Here we read the soil moisture
	i2c_cmd_handle_t cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, STEMMA_SENSOR_ADDR << 1 | READ_BIT, ACK_CHECK_EN);
	i2c_master_read(cmd, moisture, len - 1, NACK_VAL);
	i2c_master_read_byte(cmd, moisture + len - 1, NACK_VAL);
	i2c_master_stop(cmd);
	I2C_Result = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
	i2c_cmd_link_delete(cmd);

	// Check data was read and that I2C operation concluded succesfully.
	if (I2C_Result != ESP_OK)
	{
		ESP_LOGW(TAG, "Moisture reading failed");
		return I2C_Result;
	}

	// Transfer data into variable passed by reference
	*Reading = ((uint16_t)moisture[0] << 8) | moisture_data[1];

	return I2C_Result;
}

esp_err_t Read_SoilTemperature(float *Reading)
{
	esp_err_t I2C_Result;
	int len = SOIL_TEMP_DATA_LENGTH;
	uint8_t Temperature[SOIL_TEMP_DATA_LENGTH];
	

	// Send out control bytes, indicating soil temperature read request
	i2c_cmd_handle_t cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, STEMMA_SENSOR_ADDR << 1 | WRITE_BIT, ACK_CHECK_EN);
	i2c_master_write_byte(cmd, STEMMA_TEMP_BASE_REG, ACK_CHECK_EN);
	i2c_master_write_byte(cmd, STEMMA_TEMP_FUNC_REG, ACK_CHECK_EN);
	i2c_master_stop(cmd);
	I2C_Result = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS)
	i2c_cmd_link_delete(cmd);

	// Check that request passed through
	if (I2C_Result != ESP_OK) {
		ESP_LOGW(TAG, "Temperature request failed");
		return I2C_Result;
	}

	// small delay as indicated by datasheet
	delay_ms(50);

	// Begin read operations
	i2c_cmd_handle_t cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, STEMMA_SENSOR_ADDR << 1 | READ_BIT, ACK_CHECK_EN);
	i2c_master_read(cmd, Temperature, len - 1, NACK_VAL);
	i2c_master_read_byte(cmd, Temperature + len - 1, NACK_VAL);
	i2c_master_stop(cmd);
	I2C_Result = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
	i2c_cmd_link_delete(cmd);

	// check read operation concluded successfully, and store data
	if (I2C_Result == ESP_OK)
	{
		int32_t raw_temp = ((uint32_t)Temperature[0] << 24) | ((uint32_t)Temperature[1] << 16) | ((uint32_t)Temperature[2] << 8) | Temperature[3];
		*Reading = (1.0 / (1UL << 16)) * raw_temp; // normalize value
	}
	else
	{
		ESP_LOGW(TAG, "Temperature read failed");
	}

	return I2C_Result;
}

bool Read_Air_HumidityTemperature(float *Temp_Reading, float *Humid_Reading) {
	// Start reading
	if(!sht3x_start_measurement(SHT3X_DataStruct, SHT3X_Mode, SHT3X_Repeat)) {
		return false;
	}

	// Get results of (last) reading
	if(!sht3x_get_results(SHT3X_DataStruct, &SHT3X_Mode, &SHT3X_Repeat)) {
		return false;
	}

	return true;
}


#ifdef SENSOR_LIB_TEST
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
#endif // SENSOR_TEST