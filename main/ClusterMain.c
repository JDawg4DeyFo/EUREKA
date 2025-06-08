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
#include <stdio.h>
#include <inttypes.h>
#include <string.h>


#include "esp_log.h"
#include "esp_timer.h"
#include "esp_sleep.h"

// #include "../include/Memory.h"
#include "../include/Protocol.h"
#include "../include/LoRa.h"
#include <ina219.h>

// Defines
/******************************************************************************/
#define SENDING_TIMEOUT_TIME 100 // ACCURATE VALUE NEEDED. timeout for tx transmissions
#define PLACEHOLDER_UNIQUEID 102;

#define EMERGENCY_SLEEP_TIME_SEC 600 // NOTE: Should probably changed and refined
#define MICROSECOND_CONVERSION 1000000

// Voltage monitor defines
#define SHUNT_RESISTANCE 0.24
#define CRITICAL_VOLTAGE 11.0 // ACCURATE VALUE NEEDED ... minimum voltage is 10V for battery

// i2c defines NOTE: probably should be replaced with CONFIG_I2C values
#define I2C_SCL 42
#define I2C_SDA 41
#define I2C_PORT 0
// Datatypes
/******************************************************************************/
typedef struct {
	uint8_t *Buffer;
	bool *Receiving;
	bool *BufferReady;
	bool *Sending;
} LoRaTaskParams;


// Variables
/******************************************************************************/
static const char *TAG = "ClusterMain.c";
static LORA_Packet_t MainPacket, StoragePacket; // store packet is needed in case original packet
												// needs to be stored if no ack received
static bool AwaitingResponse;					// to check in main loop
static int Send_StartTime, Last_DataRequest;
static ina219_t MonitorHandle;
static uint16_t Period;
static uint32_t TempTimestamp;
uint8_t Unique_NodeID;
uint8_t TX_Buf[MAX_BUFF];
uint8_t RX_Buff[MAX_BUFF];
uint8_t rx_len, tx_len;

static uint8_t Raw_Buf[MAX_BUFF];
static bool RX_Flag, Buf_Flag, TX_Flag;

// bool TX_Buf_Empty, RX_Buf_Empty;

static LoRaTaskParams TaskFlags = {
	.Buffer = Raw_Buf,
	.Receiving = &RX_Flag,
	.BufferReady = &Buf_Flag,
	.Sending = &TX_Flag,
};

union
{
	float f;
	uint8_t b[4];
} float_converter;

// Functions
/******************************************************************************/
// LORA RX task
void task_rx(void *pvParameters) {
	// Initialize passed parameters
	LoRaTaskParams *params = (LoRaTaskParams *)pvParameters;
	uint8_t *buf = params->Buffer;
	bool *RX = params->Receiving;
	bool *buf_ready = params->BufferReady;
	bool *TX = params->Sending;

	// log start
	ESP_LOGI(pcTaskGetName(NULL), "Start");
	
	// Main loop
	while(1) {
		vTaskDelay(1); // avoiding watchdog

		// Check if the lora module is in TX use
		if(*TX == true) {
			continue;
		}

		// otherwise, continue on
		*RX = true;	// flag RX use
		uint8_t rxLen = LoRaReceive(buf, sizeof(buf));
		if (rxLen > 0) {
			*buf_ready = true;

#ifdef CONFIG_DEBUG_STUFF
			ESP_LOGI(pcTaskGetName(NULL), "%d byte packet received:[%.*s]", rxLen, rxLen, buf);

			int8_t rssi, snr;
			GetPacketStatus(&rssi, &snr);
			ESP_LOGI(pcTaskGetName(NULL), "rssi=%d[dBm] snr=%d[dB]", rssi, snr);
#endif
		}
		*RX = false; // unflag RX use
	}
}

// Get packet from RX buffer and store into main packet
bool GetPacket()
{
	// Return false if there's no packet
	if (!Buf_Flag)
	{
		return false;
	}

	// Set TX flag high to ensure buffer isn't written to by RX task
	TX_Flag = true;
	Buf_Flag = false;


	// Transfer bytes from lora buffer into main packet structure
	MainPacket.NodeID = Raw_Buf[0];
	MainPacket.Pkt_Type = Raw_Buf[1];
	memcpy(MainPacket.Timestamp, Raw_Buf + 2, TIMESTAMP_LENGTH);
	MainPacket.Length = Raw_Buf[6];
	memcpy(MainPacket.Payload, Raw_Buf + 7, MainPacket.Length);
	MainPacket.CRC = *(Raw_Buf + MainPacket.Length + 7);

	// reset flag
	TX_Flag = false;

	return true;
}

// Iterative CRC calculation
uint8_t Iterative_CRC(bool Reset, uint8_t char_in)
{
	// Checksum variable to maintain its value through iterative calls
	static uint8_t Current_Checksum;

	// Check for reset flag
	if (Reset)
	{
		Current_Checksum = 0;
	}

	// Simple crc algorithm
	Current_Checksum += char_in;
	Current_Checksum = (Current_Checksum >> 1) | (Current_Checksum << 7);

	// Return value
	return Current_Checksum;
}

// Calculate CRC for TX packet
void Calculate_CRC(LORA_Packet_t *Packet)
{
	int i;
	uint8_t TempChar;

	// Simple char calculations
	Iterative_CRC(true, Packet->NodeID);
	Iterative_CRC(false, (uint8_t)Packet->Pkt_Type);

	// 32 bit timestamp requires some finesse
	TempChar = (uint8_t)(Packet->Timestamp)[0];
	Iterative_CRC(false, TempChar);
	TempChar = (uint8_t)(Packet->Timestamp)[1];
	Iterative_CRC(false, TempChar);
	TempChar = (uint8_t)(Packet->Timestamp)[2];
	Iterative_CRC(false, TempChar);
	TempChar = (uint8_t)(Packet->Timestamp)[3];
	Iterative_CRC(false, TempChar);

	// Length
	Iterative_CRC(false, Packet->Length);

	// Payload
	for (i = 0; i < (Packet->Length - 1); i++)
	{
		Iterative_CRC(false, *(Packet->Payload + i));
	}

	// final payload and store CRC
	Packet->CRC = Iterative_CRC(false, *(Packet->Payload + Packet->Length - 1));
}

bool SendAck()
{
	uint8_t ACK_CRC;
	uint8_t buffer[MAX_PACKET_LENGTH];
	// convert packet to array of chars
	buffer[0] = PLACEHOLDER_UNIQUEID;
	buffer[1] = TX_ACK;

	TempTimestamp = 100;
	// timestamp copy
	memcpy(buffer + 2, &TempTimestamp, 4);

	// convert length
	buffer[6] = TX_ACK_LEN;

	// Calculate CRC  	NOTE: value doesn't have to be calculated ... it's the same every time
	Iterative_CRC(true, buffer[0]);
	Iterative_CRC(false, buffer[1]);
	Iterative_CRC(false, buffer[2]);
	Iterative_CRC(false, buffer[3]);
	Iterative_CRC(false, buffer[4]);
	Iterative_CRC(false, buffer[5]);
	ACK_CRC = Iterative_CRC(false, buffer[6]);

	// store CRC
	buffer[7] = ACK_CRC;
	tx_len = 8;
	
	// wait for lora module to be available
	while(RX_Flag);
	
	// Set TX flag and send
	TX_Flag = true;
	if (LoRaSend(buffer, tx_len, SX126x_TXMODE_SYNC) == false)
	{
		ESP_LOGE(TAG, "LoRaSend fail");
	}

	// reset flag
	TX_Flag = false;

	return true;
}

bool SendDebugPacket()
{
	ESP_LOGI(TAG, "Debug packet send reached");

	uint8_t ACK_CRC;
	uint8_t buffer[MAX_PACKET_LENGTH];
	// convert packet to array of chars
	buffer[0] = PLACEHOLDER_UNIQUEID;
	buffer[1] = DEBUG;

	TempTimestamp = 100;
	// timestamp copy
	memcpy(buffer + 2, &TempTimestamp, 4);

	// convert length
	buffer[6] = DEBUG_LEN;

	buffer[7] = 8;

	// Calculate CRC  	NOTE: value doesn't have to be calculated ... it's the same every time
	Iterative_CRC(true, buffer[0]);
	Iterative_CRC(false, buffer[1]);
	Iterative_CRC(false, buffer[2]);
	Iterative_CRC(false, buffer[3]);
	Iterative_CRC(false, buffer[4]);
	Iterative_CRC(false, buffer[5]);
	Iterative_CRC(false, buffer[6]);
	ACK_CRC = Iterative_CRC(false, buffer[7]);

	// store CRC
	buffer[8] = ACK_CRC;

	// send packet and set flags
	tx_len = 9;

	// wait for lora module to be available
	while(RX_Flag);
	
	// Set TX flag and send
	TX_Flag = true;
	if (LoRaSend(buffer, tx_len, SX126x_TXMODE_SYNC) == false)
	{
		ESP_LOGE(TAG, "LoRaSend fail");
	}

	// reset flag
	TX_Flag = false;

	return true;
}

// Send_Packet
bool SendPacket()
{
	uint8_t buffer[MAX_PACKET_LENGTH];

	memset(buffer, 0, sizeof(buffer));

	// convert packet to array of chars
	buffer[0] = MainPacket.NodeID;
	buffer[1] = MainPacket.Pkt_Type;

	// timestamp copy
	memcpy(buffer + 2, MainPacket.Timestamp, 4);

	// convert length
	buffer[6] = MainPacket.Length;

	// convert length
	memcpy(buffer + 7, MainPacket.Payload, MainPacket.Length);

	*(buffer + 7 + MainPacket.Length) = MainPacket.CRC;

	// send packet and set flags
	tx_len = 8 + MainPacket.Length;
	
	// wait for lora module to be available
	while(RX_Flag);
	
	// Set TX flag and send
	TX_Flag = true;
	if (LoRaSend(buffer, tx_len, SX126x_TXMODE_SYNC) == false)
	{
		ESP_LOGE(TAG, "LoRaSend fail");
	}

	// reset flag
	TX_Flag = false;

	// Init response logic
	Send_StartTime = esp_timer_get_time();
	AwaitingResponse = true;

	return true;
}

// send new period
bool SendNewPeriod()
{
	// build packet
	MainPacket.NodeID = PLACEHOLDER_UNIQUEID;
	MainPacket.Pkt_Type = PERIOD_UPDATE;

	TempTimestamp = 100; // replace with actual value later
	memcpy(MainPacket.Timestamp, &TempTimestamp, 4);

	// redundant main packet period update, but good to be safe just in case
	MainPacket.Payload[0] = BYTE_MASK & (Period >> BYTE_SHIFT);
	MainPacket.Payload[1] = BYTE_MASK & Period;

	MainPacket.Length = 2;

	Calculate_CRC(&MainPacket);

	// store packet in case it needs to get stored
	StoragePacket = MainPacket;
	SendPacket();

	return true;
}

// send data request
bool SendSensorDataRequest()
{
	// Node ID and packet ID chars
	MainPacket.NodeID = PLACEHOLDER_UNIQUEID;
	MainPacket.Pkt_Type = REQUEST_SENSOR_DATA;

	// Copy in 4 byte timestamp
	TempTimestamp = 100;
	memcpy(MainPacket.Timestamp, &TempTimestamp, 4);

	// No payload
	MainPacket.Length = 0;

	Calculate_CRC(&MainPacket);

	// Store packet win case it needs to get stored
	StoragePacket = MainPacket;
	SendPacket();
	
	return true;
}

bool StorePacket()
{
	AwaitingResponse = false;

	// write storage packet into sd card

	return true;
}

// Parse any packets
bool ParsePacket(void)
{
	// Switch case depending on packet type
	// NOTE: All cases should verify integrity of packet
	switch (MainPacket.Pkt_Type)
	{
	case NOTHING:
		break;

	// Packet contains raw sensor data
	// Future revision might contain process_sensor_data() function
	// to convert raw sensor node data into transmit friendly processed data
	case RAW_SENSOR_DATA:
		// Check if awaiting response
		if (AwaitingResponse)
		{
			StorePacket();
		}

		// Send ACK
		SendAck();

		SendPacket();
		break;

	// Packet contains a period update for sensor nodes
	case PERIOD_UPDATE:
		// Check if awaiting response
		if (AwaitingResponse)
		{
			StorePacket();
		}

		// Send ACK
		SendAck();

		// Update period
		Period = MainPacket.Payload[0] << BYTE_SHIFT;
		Period += MainPacket.Payload[1];

		// send new period
		SendNewPeriod();
		break;

	// Packet contains sensor data request
	// packet shoudn't have any payload
	// all cluster head has to do is send a sense request to sensor nodes
	// response will be handled just like any other raw sensor data packet
	// cluster head should also relay data request
	case REQUEST_SENSOR_DATA:
		// Check if awaiting response
		if (AwaitingResponse)
		{
			StorePacket();
		}

		// Send ACK
		SendAck();

		// Check that data wasn't just requested
		int Last_Request_Time = (esp_timer_get_time() - Last_DataRequest) / 1000;
		if (Last_Request_Time < DATAREQ_DEBOUNCE_MS)
		{
			break;
		}

		// Send data request
		SendSensorDataRequest();
		break;

	// Packet contains processed sensor data
	// here, the cluster head should act as a relay.
	case PROCESSED_SENSOR_DATA:
		// Check if awaiting response
		if (AwaitingResponse)
		{
			StorePacket();
		}

		// Send ACK
		SendAck();

		// Forward data
		SendPacket();
		break;

	case TIME_UPDATE:
		// Check if awaiting response
		if (AwaitingResponse)
		{
			StorePacket();
		}

		// Send ACK
		SendAck();

		// update local time
		// updatetime()
		// Foward data
		break;

	case BATTERY_DATA:
		// Check if awaiting response
		if (AwaitingResponse)
		{
			StorePacket();
		}

		// Send ACK
		SendAck();

		// Foward data
		SendPacket();

		break;

	case BATTERY_REQUEST:
		// Check if awaiting response
		if (AwaitingResponse)
		{
			StorePacket();
		}

		// Send ACK
		SendAck();

		// Foward data
		SendPacket();

		break;

	case DEBUG:
#ifdef CONFIG_DEBUG
		ESP_LOGI(TAG, "Debug packet received");
#endif

		// Check if awaiting response
		if (AwaitingResponse)
		{
			StorePacket();
		}

		// Send ACK
		SendAck();

		// Foward data
		SendPacket();

		break;

	// simple set flag low
	case TX_ACK:
		AwaitingResponse = false;
		break;

	default:
		// Check if awaiting response
		if (AwaitingResponse)
		{
			StorePacket();
		}

		// Send ACK
		SendAck();

		// Foward data
		SendPacket();

		break;
	}

	return true;
}

// Could add lora send functions here if needed.

// main()
/******************************************************************************/
void app_main(void)
{
	// init variables
	Period = DEFAULT_PERIOD;
	Unique_NodeID = PLACEHOLDER_UNIQUEID; // place holder value

	// Power_init();
	ina219_init_desc(&MonitorHandle, INA219_ADDR_GND_GND, I2C_PORT, I2C_SDA, I2C_SCL);
	ina219_init(&MonitorHandle);
	ina219_configure(&MonitorHandle, INA219_BUS_RANGE_32V, INA219_GAIN_0_125, INA219_RES_12BIT_1S, INA219_RES_12BIT_1S, INA219_MODE_CONT_SHUNT_BUS);
	ina219_calibrate(&MonitorHandle, SHUNT_RESISTANCE);

	// Lora init
	LoRaInit();
	int8_t txPowerInDbm = 22;

	// set frequency
	uint32_t frequencyInHz = 0;
#if CONFIG_433MHZ
	frequencyInHz = 433000000;
	ESP_LOGI(TAG, "Frequency is 433MHz");
#elif CONFIG_866MHZ
	frequencyInHz = 866000000;
	ESP_LOGI(TAG, "Frequency is 866MHz");
#elif CONFIG_915MHZ
	frequencyInHz = 915000000;
	ESP_LOGI(TAG, "Frequency is 915MHz");
#elif CONFIG_OTHER
	ESP_LOGI(TAG, "Frequency is %dMHz", CONFIG_OTHER_FREQUENCY);
	frequencyInHz = CONFIG_OTHER_FREQUENCY * 1000000;
#endif

	// txco power configurations for LORA
#if CONFIG_USE_TCXO
	ESP_LOGW(TAG, "Enable TCXO");
	float tcxoVoltage = 3.3;	 // use TCXO
	bool useRegulatorLDO = true; // use DCDC + LDO
#else
	ESP_LOGW(TAG, "Disable TCXO");
	float tcxoVoltage = 0.0;	  // don't use TCXO
	bool useRegulatorLDO = false; // use only LDO in all modes
#endif

	// begin the lora module
	if (LoRaBegin(frequencyInHz, txPowerInDbm, tcxoVoltage, useRegulatorLDO) != 0)
	{
		ESP_LOGE(TAG, "Does not recognize the module");
		while (1)
		{
			vTaskDelay(1);
		}
	}

	// NOTE: These variables could and maybe should be configured in the menuconfig
	uint8_t spreadingFactor = 12;
	uint8_t bandwidth = 4;
	uint8_t codingRate = 1;
	uint16_t preambleLength = 8;
	uint8_t payloadLen = 0;
	bool crcOn = true;
	bool invertIrq = false;
#if CONFIG_ADVANCED
	spreadingFactor = CONFIG_SF_RATE;
	bandwidth = CONFIG_BANDWIDTH;
	codingRate = CONFIG_CODING_RATE;
#endif
	LoRaConfig(spreadingFactor, bandwidth, codingRate, preambleLength, payloadLen, crcOn, invertIrq);

	// esp_timer_init() // apparently this is already initialized

	// Start RX
	xTaskCreate(&task_rx, "RX", 1024*4, &TaskFlags, 5, NULL);

	// main program
	int IterationCount = 0;
	while (1)
	{	
#ifdef CONFIG_DEBUG_STUFF
		// One iteration takes about 250 us
		// int start;
		// start = esp_timer_get_time();
#endif
		IterationCount ++;
		int IterationTime;
		float BusVoltage;

		// Check for packets
		if (GetPacket()) {
			ParsePacket();
		}

		// iteration time in ms
		IterationTime = (esp_timer_get_time() - Send_StartTime) / 1000;
		if (AwaitingResponse && (IterationTime > RESPONSE_TIMEOUT_MS))
		{
			// store stuff
			ESP_LOGW(TAG, "No response from node");
			StorePacket();
		}

		// check power
		ina219_get_bus_voltage(&MonitorHandle, &BusVoltage);
		if (BusVoltage < CRITICAL_VOLTAGE)
		{
			// ESP_LOGE(TAG, "Below critical voltage!");
			// update network that cluster head is shutting off?

			// shut off
			uint64_t wakeup_time = EMERGENCY_SLEEP_TIME_SEC * MICROSECOND_CONVERSION;
			esp_sleep_enable_timer_wakeup(wakeup_time);
			// ESP_LOGI(TAG, "Entering deep sleep for %d seconds...", EMERGENCY_SLEEP_TIME_SEC);

			// Light sleep start
			// esp_deep_sleep_start(); // deep sleep mode but LoRa wakeup is enabled
		}

#ifdef CONFIG_DEBUG_STUFF
		// int end = esp_timer_get_time();
		// ESP_LOGI(TAG, "One loop took %d us", end - start);
		if(IterationCount > 20000) {
			// should just be replaced with a parallel tx task probably...
		// would definitley be hard to cordinate the timing and hardware constraints tho
		// SendDebugPacket();
		SendSensorDataRequest();
		}
#endif
	}
}
