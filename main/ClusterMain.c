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

// Defines
/******************************************************************************/
#define SENDING_TIMEOUT_TIME 100 				// ACCURATE VALUE NEEDED. timeout for tx transmissions
// Datatypes
/******************************************************************************/
typedef enum {
	NOTHING,
	RAW_SENSOR_DATA,
	PERIOD_UPDATE,
	REQUEST_SENSOR_DATA,
	PROCESSED_SENSOR_DATA,
	TX_ACK
} PacketIDs_t

// Typedefs
/******************************************************************************/
typedef struct {
	unsigned char *Payload;
	PacketIDs_t Pkt_Type;
	bool Ready;
} LORA_Packet_t;

// Variables
/******************************************************************************/
static const char *TAG = "ClusterMain.c";
static sx1262_handle_t LORA_Handle;
static LORA_Packet_t MainPacket;
static bool Sending, Response; // to check in main loop
static int Send_StartTime;
static float Period;

// Functions
/******************************************************************************/
// LORA ISR

// Send_Packet

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

			// send new period
			Sending = true;
			Send_StartTime = esp_timer_get_time();
			// SendNewPeriod();
			break;

		// Packet contains sensor data request
		// packet shoudn't have any payload
		// all cluster head has to do is send a sense request to sensor nodes
		// response will be handled just like any other raw sensor data packet
		case REQUEST_SENSOR_DATA:
			// Send data request
			Sending = true;
			Send_StartTime = esp_timer_get_time();
			// SendDataRequest();
			break;

		// Packet contains processed sensor data
		// here, the cluster head should act as a relay.
		case PROCESSED_SENSOR_DATA:
			// Forward data
			Sending = true;
			Send_StartTime = esp_timer_get_time();
			// Sensordata();
			break;
			
	}

	return true;
}

// Could add lora send functions here if needed.

// main()
/******************************************************************************/
void app_main(void) {
	// Configuration and initialization
	esp_timer_init();
	Sending = false;
	
	sx1262_lora_begin(LORA_Handle);
	sx1262_lora_set_continuous_receive_mode(LORA_Handle);
	
	// init power monitor

	// main program
	while (1) {
		// Poll packets and parse
		if (MainPacket.Ready == true) {
			ParsePacket();
		}

		// If transmitting, check for timeout or response
		while (Sending) {
			// Timeout condition
			// here we will store data in memory for later transmission
			if((esp_timer_get_time() - Sending_StartTime) > SENDING_TIMEOUT_TIME) {
				// lora stop sending();
				sending = false;
				// memory store data();
			}
			

			if(Response) {
				// if more TX packets
					// send and reset start time
				// else{}: code below
				// lora stop sending();
				sending = false;
			}

		}
	}
}
