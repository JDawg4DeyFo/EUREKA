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

// Datatypes
/******************************************************************************/
typedef enum {
	NOTHING,
	RAW_SENSOR_DATA,
	PERIOD_UPDATE,
	REQUEST_SENSOR_DATA,
	PROCESSED_SENSOR_DATA,
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

static LORA_Packet_t MainPacket;

static bool Sending, Timeout; // to check in main loop

// Functions
/******************************************************************************/
// LORA ISR


// Parse any packets
bool ParsePacket(void) {
	// switch(packet id)

	// update period info

	// send info

	// request data

	// send data

	// if sending:
		// set sending = true
		// log start time

	return true;
}

// Could add lora send functions here if needed.

// main()
/******************************************************************************/
void app_main(void) {
	// config and initialize
	Sending = false;
		// init sensors
		// init lora
		// init power monitor

	// main program
	while (1) {
		// Poll packets and parse
		if (MainPacket.Ready == true) {
			ParsePacket();
		}

		// If transmitting, check for timeout or response
		while (Sending && !Timeout) {
			// if duration > timeout time
				// timeout = true
				// stop sending
				// sending = false
				// store data

			// if response
				// stop sending
				// sending = false
		}

		Timeout = false;
	}
}
