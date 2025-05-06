/**
 * @file SesnorMain.c
 * @author Jacob Dennon (jdennon@ucsc.edu)
 * @brief Main program for sensor node operation
 * @version 0.1
 * @date 2025-03-08
 * 
 * @copyright Copyright (c) 2025
 * 
 */

// #includes
/******************************************************************************/
#include <stddef.h>

#include "esp_log.h"
#include "esp_timer.h"

#include "../include/Sensors.h"
#include "../include/LoRa_main.h"
#include "../include/Protocol.h"

// #defines
/******************************************************************************/
#define DEFAULT_PERIOD 10					// Default period in seconds
#define MICROSECOND_TO_SECOND 1000000
#define TIMEOUT_PERIOD 30					// timeout period in seconds

// Data types
/******************************************************************************/
typedef struct {
	float WindDirection;
	float Temperature;
	float Humidity;
	float WindSpeed;
	short Soil_Moisture;
	float Soil_Temperature;
} SensorData_t;

// Variables
/******************************************************************************/
static float Period;
static sx1262_handle_t LORA_Handle;
static LORA_Packet_t MainPacket;
static bool Sending, Response, MainPacket_Ready;
static int Sending_StartTime;
SensorData_t SensorData;


// Functions
/******************************************************************************/
// Return: true for sucess
// 		   false for fail
bool SenseData() {
	float TempFloat1, TempFloat2;
	short TempShort;
	// Initialize return value to true... will be set false on first fail
	bool ret = true;

	// Try reading soil moisture
	if(Read_SoilMoisture(&TempShort) != ESP_OK) {
		ret = false;
	}
	SensorData.Soil_Moisture = TempShort;

	// Read wind speed and direction. No fail condition for these functions
	SensorData.Soil_Moisture = Get_Wind_Speed();
	SensorData.WindDirection = Get_Wind_Direction();

	// Read humidity and temperature
	if(!Read_SHT30_HumidityTemperature(&TempFloat1, &TempFloat2)) {
		ret = false;
	}

	return ret;
}


bool ParsePacket() {
	// The sensor node only has to respond to a few packet types
	switch (MainPacket.Pkt_Type) {
		case PERIOD_UPDATE:
			// Access new period from payload
			//update period
			// Period = Newperiod;
			break;

		case REQUEST_SENSOR_DATA:
			// sense data
			// SenseData();
			// build packet
			// Build Lora packet();
			// send packet
			// SendLoraPacket();
			Sending = true;
			Sending_StartTime = esp_timer_get_time();

		// for all other cases, break
		default:
			break;
	}

	// Reset ready flag
	MainPacket_Ready = false;
}


// SPI interrupt (if not handled by LoRa lib)

void app_main(void)
{
	// Initialization and configuration
	Sensors_Init(ALL_SENSORS);
	// Power_init();
	sx1262_lora_begin(&LORA_Handle);
	sx1262_lora_set_continuous_receive_mode(&LORA_Handle);
	//init timer
	esp_timer_init();

	// init variables
	Sending = false;
	Response = false;
	Period = DEFAULT_PERIOD;
	
	while(1) {
		// check incoming packets
		if (MainPacket_Ready) {
			ParsePacket();
		}

		// If transmitting, transmit untill timeout or TX ack
		while(Sending) {
			// timeout condition
			if ((esp_timer_get_time() - Sending_StartTime) > (TIMEOUT_PERIOD * MICROSECOND_TO_SECOND)) {
				// lora stop sending();
				Sending = false;
			}

			// Response condition
			if (Response) {
				// lora stop sending();
				Sending = false;
				Response = false;
			}
		}

		// Go to sleep for period
		esp_sleep_enable_timer_wakeup(Period * MICROSECOND_TO_SECOND);
		esp_deep_sleep_start();
	}
}