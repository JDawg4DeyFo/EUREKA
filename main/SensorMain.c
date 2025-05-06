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
#define BYTE_SHIFT 8						
#define BYTE_MASK 0xFF

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

union {
	float f;
	uint8_t b[4];
} float_converter;

// Variables
/******************************************************************************/
static short Period;
static sx1262_handle_t LORA_Handle;
static LORA_Packet_t MainPacket;
static bool Sending, Response, MainPacket_Ready;
static int Sending_StartTime;
SensorData_t SensorData;
static uint8_t Unique_NodeID;


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


// Iterative CRC calculation
uint8_t Iterative_CRC(bool Reset, uint8_t char_in){
	// Checksum variable to maintain its value through iterative calls
	static uint8_t Current_Checksum;
	
	// Check for reset flag
	if(Reset) {
		Current_Checksum = 0;
	}

	// Simple crc algorithm
	Current_Checksum += char_in;
	Current_Checksum = (Current_Checksum >> 1) | (Current_Checksum << 7);

	// Return value
	return Current_Checksum;
}

// Calculate CRC for TX packet
void Calculate_CRC(LORA_Packet_t *Packet) {
	int i;
	uint8_t TempChar;
	
	// Simple char calculations
	Iterative_CRC(true, Packet->NodeID);
	Iterative_CRC(false, (uint8_t)Packet->Pkt_Type);

	// 32 bit timestamp requires some finesse
	TempChar = (uint8_t)((Packet->Timestamp>>24) & 0xFF);
	Iterative_CRC(false, TempChar);
	TempChar = (uint8_t)((Packet->Timestamp>>16) & 0xFF);
	Iterative_CRC(false, TempChar);
	TempChar = (uint8_t)((Packet->Timestamp>>8) & 0xFF);
	Iterative_CRC(false, TempChar);
	TempChar = (uint8_t)(Packet->Timestamp & 0xFF);
	Iterative_CRC(false, TempChar);

	// Length
	Iterative_CRC(false, Packet->Length);

	// Payload
	for (i = 0; i < (Packet->Length - 1); i++) {
		Iterative_CRC(false, *(Packet->Payload + i));
	}

	// final payload and store CRC
	Packet->CRC = Iterative_CRC(false, *(Packet->Payload + Packet->Length - 1));
}

bool SendPacket() {
	uint8_t buffer[MAX_PACKET_LENGTH];
	
	// convert packet to array of chars
	buffer[0] = MainPacket.NodeID;
	buffer[1] = MainPacket.Pkt_Type;
	
	// timestamp copy
	memcpy(buffer[2], MainPacket.Timestamp, 4);

	// convert length
	buffer[4] = MainPacket.Length;

	// convert length
	memcpy(buffer[5], MainPacket.Payload, MainPacket.Length);

	*(buffer + 5 + MainPacket.Legnth) = MainPacket.CRC;
	
	Sending = true;
	Sending_StartTime = esp_timer_get_time();

	return true;
}


bool ParsePacket() {
	// The sensor node only has to respond to a few packet types
	switch (MainPacket.Pkt_Type) {
		case PERIOD_UPDATE:
			// Access new period from payload
			//update period
			Period = MainPacket.Payload[0] << BYTE_SHIFT;
			Period += MainPacket.Payload[1];
			break;

		case REQUEST_SENSOR_DATA:
			// sense data
			SenseData();

			// build packet
			MainPacket.NodeID = Unique_NodeID;
			MainPacket.Pkt_Type = RAW_SENSOR_DATA;
			MainPacket.Timestamp = 100; // PLACEHOLDER!! REPLACE WITH REAL TIME
			MainPacket.Length = RAW_SENSOR_DATA_LEN;

			// Store payload
			// structure is in pag 104 of jacob's eng notebook
			MainPacket.Payload[0] = SensorData.Soil_Moisture >> BYTE_SHIFT;
			MainPacket.Payload[1] = SensorData.Soil_Moisture & BYTE_MASK;
			
			// float conversion for soil temperature
			float_converter.f = SensorData.Soil_Temperature;
			memcpy(MainPacket.Payload[2], float_converter.b, 4);

			// float conversion for humidity
			float_converter.f = SensorData.Humidity;
			memcpy(MainPacket.Payload[6], float_converter.b, 4);

			// float conversion for temperature
			float_converter.f = SensorData.Temperature;
			memcpy(MainPacket.Payload[10], float_converter.b, 4);

			// float conversion of wind speed
			float_converter.f = SensorData.WindSpeed;
			memcpy(MainPacket.Payload[14], float_converter.b, 4);

			// float conversion of wind direction
			float_converter.f = SensorData.WindSpeed;
			memcpy(MainPacket.Payload[18], float_converter.b, 4);

			return Send_MainPacket();

		// for all other cases, break
		default:
			break;
	}

	// Reset ready flag
	MainPacket_Ready = false;

	return true;
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

	// assign unique node id
	Unique_NodeID = 101; // place holder value

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