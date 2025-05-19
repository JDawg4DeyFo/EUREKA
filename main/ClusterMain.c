/**
 * @file ClusterMain.c
 * @author Jacob Dennon (jdennon@ucsc.edu)
 * @brief Main program for cluster head.
 * @version 0.1
 * @date 2025-03-08
 * 
 * @copyright Copyright (c) 2025
 * 
 */


// Includes
/******************************************************************************/
#include <stddef.h>
#include "esp_log.h"
#include "esp_timer.h"

#include "../include/LoRa_main.h"
// #include "../include/Memory.h"
#include "../include/Protocol.h"

// Defines
/******************************************************************************/
#define SENDING_TIMEOUT_TIME 100 				// ACCURATE VALUE NEEDED. timeout for tx transmissions
// Datatypes
/******************************************************************************/

// Variables
/******************************************************************************/
static const char *TAG = "ClusterMain.c";
static sx1262_handle_t LORA_Handle;
static LORA_Packet_t MainPacket;
static bool Sending, Response; // to check in main loop
static int Send_StartTime;
static float Period;
static uint32_t TempTimestamp;

union {
	float f;
	uint8_t b[4];
} float_converter;


// Functions
/******************************************************************************/
// LORA ISR


// Get packet from RX buffer and store into main packet
bool GetPacket() {
	// return false if handle isn't initialized
	if (LORA_Handle == NULL) {
		return false;
	}

	// Transfer bytes from lora buffer into main packet structure
	MainPacket.NodeID = LORA_Handle.receive_buf[0];
	MainPacket.Pkt_Type = LORA_Handle.receive_buf[1];
	memcpy(MainPacket.Timestamp, LORA_Handle.receive_buf + 2, TIMESTAMP_LENGTH);
	MainPacket.Length = LORA_Handle.receive_buf[6];
	memcpy(MainPacket.Payload, LORA_Handle.receive_buf + 7, MainPacket.Length);
	MainPacket.CRC = *(LORA_Handle.receive_buf + MainPacket.Length + 7);

	return true;
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

// Send_Packet
bool SendPacket() {
	uint8_t buffer[MAX_PACKET_LENGTH];
	uint8_t len;
	
	// convert packet to array of chars
	buffer[0] = MainPacket.NodeID;
	buffer[1] = MainPacket.Pkt_Type;
	
	// timestamp copy
	memcpy(buffer[2], MainPacket.Timestamp, 4);

	// convert length
	buffer[6] = MainPacket.Length;

	// convert length
	memcpy(buffer[7], MainPacket.Payload, MainPacket.Length);

	*(buffer + 7 + MainPacket.Legnth) = MainPacket.CRC;
	
	// send packet and set flags
	sx1262_lora_send(&LORA_Handle, buffer, len);
	Sending = true;
	Sending_StartTime = esp_timer_get_time();

	return true;
}

// send new period
bool SendNewPeriod() {
	// build packet
	MainPacket.NodeID = Unique_NodeID;
	MainPacket.Pkt_Type = PERIOD_UPDATE;

	TempTimestamp = 100;	// replace with actual value later
	memcpy(MainPacket.Timestamp, TempTimestamp, 4);

	MainPacket.Length = 0;

	Calculate_CRC(&MainPacket);

	SendPacket();

	return true;
}

// send data request
bool SensorDataRequest() {

	return true;
}

// Parse any packets
bool ParsePacket(void) {
	// Switch case depending on packet type
	// NOTE: All cases should verify integrity of packet
	switch (MainPacket.Pkt_Type) {
		case NOTHING:
			break;
		
		// Packet contains raw sensor data
		case RAW_SENSOR_DATA:
			// process data
			// ProcessRawSensorData();
			
			// send data
			Sending = true;
			Send_StartTime = esp_timer_get_time();
			// SendSensorData();
			break;

		// Packet contains a period update for sensor nodes
		case PERIOD_UPDATE:
			// Update period
			// Period = some value, extracted from payload;
			// not sure if this is needed

			// send new period
			SendNewPeriod();
			break;

		// Packet contains sensor data request
		// packet shoudn't have any payload
		// all cluster head has to do is send a sense request to sensor nodes
		// response will be handled just like any other raw sensor data packet
		case REQUEST_SENSOR_DATA:
			// Send data request
			SendSensorDataRequest();
			break;

		// Packet contains processed sensor data
		// here, the cluster head should act as a relay.
		case PROCESSED_SENSOR_DATA:
			// Forward data
			SendPacket();
			break;
			
	}

	return true;
}

// Could add lora send functions here if needed.

// main()
/******************************************************************************/
void app_main(void) {
	// Power_init();
	sx1262_lora_begin(&LORA_Handle);
	sx1262_lora_set_continuous_receive_mode(&LORA_Handle);
	//init timer
	esp_timer_init();

	// assign unique node id
	Unique_NodeID = 102; // place holder value

	// init variables
	Sending = false;
	Response = false;
	Period = DEFAULT_PERIOD;

	// main program
	while (1) {
		// Poll packets and parse
		if (MainPacket_Ready == true) {
			GetPacket();
			ParsePacket();
		}

		// If transmitting, check for timeout or response
		while (Sending) {
			// timeout condition
			if ((esp_timer_get_time() - Sending_StartTime) > (TIMEOUT_PERIOD * MICROSECOND_TO_SECOND)) {
				// Stop sending
				sx1262_lora_set_continuous_receive_mode(&LORA_Handle);
				Sending = false;
			}

			// Response condition
			if (Response) {
				// if more TX packets
					// send and reset start time

				// else{}: code below
				sx1262_lora_set_continuous_receive_mode(&LORA_Handle);
				Sending = false;
				Response = false;
			}

		}
	}
}
