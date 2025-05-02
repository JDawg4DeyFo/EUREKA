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
#include "../../include/SHT3X.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_timer.h"


#include "driver/i2c_master.h"
#include "esp_adc/adc_oneshot.h"

// VARIABLES
/******************************************************************************/
/******************************************************************************/
static const char *TAG = "Sensors";

// Whether or not init function has already been called
static uint8_t Already_Called = 0;

// Wind direction lookup tables
static float WindDirection_LookupTable[NUMBER_OF_KEYS] = {
	2.53,
	1.31,
	1.49,
	0.27,
	0.30,
	0.21,
	0.60,
	0.41,
	0.93,
	0.79,
	2.03,
	1.93,
	3.05,
	2.67,
	2.86,
	2.26
};

// ADC max reading
static float Max_ADC_Reading = pow(2, ADC_BITWIDTH);

// Timer duration
static int Duration = 0;

// Handles and configurations
/******************************************************************************/
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

// ADC Handles and configs
static adc_oneshot_unit_handle_t ADC_Handle;

static adc_oneshot_unit_init_cfg_t ADC_Init_cfg = {
	.unit_id = ADC_UNIT_2,
	.ulp_mode = ADC_ULP_MODE_DISABLE,
};

static adc_oneshot_chan_cfg_t ADC_cfg = {
	.bitwidth = ADC_BITWIDTH_DEFAULT,
	.atten = ADC_ATTEN_DB_12,
};

static pcnt_unit_handle_t PCNT_Unit = NULL;
static pcnt_unit_config_t PCNT_Unit_cfg = {
	.high_limit = PCNT_HIGH_LIMIT,
	.low_limit = PCNT_LOW_LIMIT,
};

// PCNT handles and config
static pcnt_channel_handle_t PCNT_Channel = NULL;
static pcnt_chan_config_t PCNT_Channel_cfg = {
	.edge_gpio_num = ANEMOMETER_GPIO,
};

// DATA STRUCTURES
/******************************************************************************/
static PCNT_State_t PCNT_State = {
	.IterationCount = 0,
	.StartTime = 0,
	.EndTime = 0,
	.PCNTHandle = &PCNT_Unit,
};

static bool PCNT_CallbackLogic(pcnt_unit_handle_t unit, const pcnt_watch_event_data_t *edata, void *user_ctx) {
	// Pass context to internal variable
	PCNT_State_t *state = (PCNT_State_t *)user_ctx;

	if(state->IterationCount == 0) {
		state->IterationCount++;
		state->StartTime = esp_timer_get_time();
	} else {
		state->IterationCount = 0;
		state->EndTime = esp_timer_get_time();
		ESP_ERROR_CHECK_WITHOUT_ABORT(pcnt_unit_stop(*(state->PCNTHandle)));
		ESP_ERROR_CHECK_WITHOUT_ABORT(pcnt_unit_clear_count(*(state->PCNTHandle)));
	}

	return pdFALSE;
}

static pcnt_event_callbacks_t PCNT_Callbacks = {
	.on_reach = PCNT_CallbackLogic,
};

// FUNCTIONS
/******************************************************************************/
/******************************************************************************/

// standard delay :P
static void delay_ms(int ms)
{
    vTaskDelay((ms) / portTICK_PERIOD_MS);
}


SensorsIDs_t Sensors_Init(SensorsIDs_t Sensors)
{
	SensorsIDs_t ReturnStatus;

	// First step will be to Initialize the I2C Bus.
	// Inititialize I2C Bus, if it hasn't been initialized already.
	if (!Already_Called)
	{
		
		ESP_ERROR_CHECK_WITHOUT_ABORT(i2c_new_master_bus(&i2c_bus_config, &Bus_Handle));
	}
	// If I2C_Init() passes, set Already_Called to 1.
	Already_Called = 1;

	// Initialize sensors:
	// Each device must be added to master bus, and some devices may require
	// initialization commands (doubtful, though)
	ReturnStatus = 0;
	if (Sensors && SOIL)
	{
		ESP_ERROR_CHECK_WITHOUT_ABORT(i2c_master_bus_add_device(Bus_Handle, &Soil_Cfg, &Soil_Handle));
		
		ReturnStatus |= SOIL;
	}

	if (Sensors && WINDVANE)
	{
		// Initialization code:
		//	1. Initialize ADC Module
		ESP_ERROR_CHECK_WITHOUT_ABORT(adc_oneshot_new_unit(&ADC_Init_cfg, &ADC_Handle));
		ESP_ERROR_CHECK_WITHOUT_ABORT(adc_oneshot_config_channel(ADC_Handle, ADC_CHANNEL_1, &ADC_cfg));
		//  2. Verify connectivity of sensor
		ReturnStatus |= WINDVANE;
	}

	if (Sensors && ANEMOMETER)
	{
		// Initialize free running timer
		esp_timer_init();

		
		// Initialize PCNT
		ESP_ERROR_CHECK_WITHOUT_ABORT(pcnt_new_unit(&PCNT_Unit_cfg, &PCNT_Unit));
		ESP_ERROR_CHECK_WITHOUT_ABORT(pcnt_new_channel(PCNT_Unit, &PCNT_Channel_cfg, &PCNT_Channel));

		// Configure channel behavior
		ESP_ERROR_CHECK_WITHOUT_ABORT(pcnt_channel_set_edge_action(PCNT_Channel, PCNT_CHANNEL_EDGE_ACTION_INCREASE, PCNT_CHANNEL_LEVEL_ACTION_HOLD));
		ESP_ERROR_CHECK_WITHOUT_ABORT(pcnt_unit_add_watch_point(PCNT_Unit, 1));	// log start
		ESP_ERROR_CHECK_WITHOUT_ABORT(pcnt_unit_add_watch_point(PCNT_Unit, 2));	// log time elapsed
		ESP_ERROR_CHECK_WITHOUT_ABORT(pcnt_unit_register_event_callbacks(PCNT_Unit, &PCNT_Callbacks, &PCNT_State));

		// Start channel up
		ESP_ERROR_CHECK_WITHOUT_ABORT(pcnt_unit_enable(PCNT_Unit));
		ESP_ERROR_CHECK_WITHOUT_ABORT(pcnt_unit_stop(PCNT_Unit));
		ESP_ERROR_CHECK_WITHOUT_ABORT(pcnt_unit_clear_count(PCNT_Unit));

		ReturnStatus |= ANEMOMETER;
	}

	// Sensor 2: SHT30
	if (Sensors && SHT30)
	{
		ESP_ERROR_CHECK_WITHOUT_ABORT(i2c_master_bus_add_device(Bus_Handle, &SHT30_Cfg, &SHT30_Handle));
		
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

	ESP_ERROR_CHECK_WITHOUT_ABORT(i2c_master_transmit(Soil_Handle, Write_Buffer, sizeof(Write_Buffer), I2C_MASTER_TIMEOUT_MS));

	delay_ms(50);

	ESP_ERROR_CHECK_WITHOUT_ABORT(i2c_master_receive(Soil_Handle, Read_Buffer, Read_Buffer_Size, I2C_MASTER_TIMEOUT_MS));

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
	
	Write_Buffer[0] = STEMMA_TEMP_BASE_REG;
	Write_Buffer[1] = STEMMA_TEMP_FUNC_REG;

	ESP_LOGI(TAG, "Made it to line 156");

	ESP_ERROR_CHECK_WITHOUT_ABORT(i2c_master_transmit(Soil_Handle, Write_Buffer, sizeof(Write_Buffer), I2C_MASTER_TIMEOUT_MS));	

	delay_ms(50);

	ESP_ERROR_CHECK_WITHOUT_ABORT(i2c_master_receive(Soil_Handle, Read_Buffer, Read_Buffer_Size, I2C_MASTER_TIMEOUT_MS));

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

float Get_Wind_Direction() {
	int Reading, Key, i;
	float Direction;
	float SmallestDistance, IterationDistance, Voltage;
	
	// Procedure:
	// 1. Read raw adc
	ESP_ERROR_CHECK_WITHOUT_ABORT(adc_oneshot_read(ADC_Handle, ADC_CHANNEL_1, &Reading));

	// 2. convert to voltage
	Voltage = (Reading / Max_ADC_Reading) * MAX_ADC_VOLTAGE;

	// 3. convert to degrees
	SmallestDistance = 10; // arbitrary value
	Key = 0;
	// Iterate through lookup table to find which voltage matches the best with
	// reading.
	for (i = 0; i < NUMBER_OF_KEYS; i++) {
		IterationDistance = fabs(Voltage - WindDirection_LookupTable[i]);
		// if current iteration is better match than previous ones, set it as
		// the current best match, and update key value.
		if (IterationDistance < SmallestDistance) {
			SmallestDistance = IterationDistance;
			Key = i;
		}
	}

	// Convert key value into direction
	Direction = Key * KEY_TO_DEG;

	return Direction;
}


float Get_Wind_Speed(void) {
	float Speed;
	// if iteration  = 1, then system is currently measuring speed. in that case
	// just return previous measurement
	if (PCNT_State.IterationCount == 1) {
		// conversion code
		Speed = Duration - 1;
		return Speed;
	}
	
	// Otherwise, return previous measurement and start a new measurement
	// also update duration
	Duration = PCNT_State.EndTime - PCNT_State.StartTime;
	
	ESP_ERROR_CHECK_WITHOUT_ABORT(pcnt_unit_clear_count(PCNT_Unit));
	ESP_ERROR_CHECK_WITHOUT_ABORT(pcnt_unit_start(PCNT_Unit));

	return 0;
}

bool Deinitialize_Sensors(void) {
	// Indicate that sensors have been deinitialized
	Already_Called = 0;
	// Deinit I2C
	// remove devices
	ESP_ERROR_CHECK_WITHOUT_ABORT(i2c_master_bus_rm_device(Soil_Handle));
	ESP_ERROR_CHECK_WITHOUT_ABORT(i2c_master_bus_rm_device(SHT30_Handle));

	// remove bus
	ESP_ERROR_CHECK_WITHOUT_ABORT(i2c_del_master_bus(Bus_Handle));
	
	// Deinit ADC
	adc_oneshot_del_unit(ADC_Handle);

	// deinit pulse count
	pcnt_unit_disable(PCNT_Unit);
	pcnt_del_channel(PCNT_Channel);
	pcnt_del_unit(PCNT_Unit);

	// deinit gptimer
	// FreeRunningTimer_Deinit();

	return true;
}


