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
	LORA_Packets *next_pkt;
	LORA_Packets *last_pkt;
	PacketIDs_t Pkt_Type;
} LORA_Packets;

// Variables
/******************************************************************************/
static const char *TAG = "ClusterMain.c";

// Functions
/******************************************************************************/
// LORA ISR

// function parser

// main()
/******************************************************************************/
void app_main(void) {
	// config and initialize
		// init sensors
		// init lora
		// init power monitor

	// main program
	while (1) {
		// parse new packet
	}
}
