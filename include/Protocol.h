/**
 * @file Protocol.h
 * @author Jacob Dennon (jdennon@ucsc.edu)
 * @brief File that includes the protocol information for EUREKA packet system
 * 			not RoCa
 * @version 0.1
 * @date 2025-05-05
 * 
 * @copyright Copyright (c) 2025
 * 
 */

// #defines
/******************************************************************************/
#define MAX_PACKET_LENGTH 100
#define MAX_PAYLOAD_LENGTH 100
#define TIMESTAMP_LENGTH 4

// payload lengths
#define RAW_SENSOR_DATA_LEN 22
#define PERIOD_UPDATE_LEN	2
#define REQUEST_SENSOR_DATA_LEN 0
#define PROCESSED_SENSOR_DATA_LEN 22 // may not need this...
#define TX_ACK_LEN 0

// Typedefs
/******************************************************************************/

typedef enum {
	NOTHING,
	RAW_SENSOR_DATA,
	PERIOD_UPDATE,
	REQUEST_SENSOR_DATA,
	PROCESSED_SENSOR_DATA,
	TIME_UPDATE,	// relay and update time across nodes
	BATTERY_DATA,
	TX_ACK
} PacketIDs_t

typedef struct {
	unsigned char NodeID;
	PacketIDs_t Pkt_Type;
	unsigned char Timestamp[TIMESTAMP_LENGTH];
	unsigned char Length;
	unsigned char Payload[MAX_PAYLOAD_LENGTH];
	uint8_t CRC;
} LORA_Packet_t;