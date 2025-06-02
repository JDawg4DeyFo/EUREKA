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

typedef struct {
	uint8_t *Buffer;
	bool *Receiving;
	bool *BufferReady;
	bool *Sending;
} LoRaTaskParams;

// Variables
/******************************************************************************/
static uint16_t Period;
static sx1262_handle_t LORA_Handle;
static LORA_Packet_t MainPacket;
static bool Sending, Response, MainPacket_Ready;
static int Sending_StartTime;
SensorData_t SensorData;
static uint8_t Unique_NodeID;
static uint32_t TempTimestamp;


static uint8_t Raw_Buf[MAX_BUFF];
static bool RX_Flag, Buf_Flag, TX_Flag;

// bool TX_Buf_Empty, RX_Buf_Empty;

static LoRaTaskParams TaskFlags = {
	.Buffer = Raw_Buf,
	.Receiving = &RX_Flag,
	.BufferReady = &Buf_Flag,
	.Sending = &TX_Flag,
};

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
	// store reading
	SensorData.Soil_Moisture = TempShort;

	// Read wind speed and direction. No fail condition for these functions
	SensorData.Soil_Moisture = Get_Wind_Speed();
	SensorData.WindDirection = Get_Wind_Direction();

	// Read humidity and temperature
	if(!Read_SHT30_HumidityTemperature(&TempFloat1, &TempFloat2)) {
		ret = false;
	}
	// Store readings
	SensorData.Temperature = TempFloat1;
	SensorData.Humidity = TempFloat2;

	// read particle data
	// ReadParticle()

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

bool SendMainPacket() {
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

// Get packet from RX buffer and store into main packet
bool GetPacket() {
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

bool ParsePacket() {
	// The sensor node only has to respond to a few packet types
	switch (MainPacket.Pkt_Type) {
		case PERIOD_UPDATE:
			// Access new period from payload
			//update period
			Period = MainPacket.Payload[0] << BYTE_SHIFT;
			Period += MainPacket.Payload[1];

			// Acknowledge Packet
			SendAck();

			break;

		case REQUEST_SENSOR_DATA:
			// sense data
			SenseData();

			// build packet
			MainPacket.NodeID = Unique_NodeID;
			MainPacket.Pkt_Type = RAW_SENSOR_DATA;

			TempTimestamp = 100;		// replace with actual stamp later
			memcpy(MainPacket.Timestamp, TempTimestamp, 4);
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

			// Calculate and store CRC
			Calculate_CRC(&MainPacket);

			// Acknowledge Packet
			SendAck();

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
	ina219_init_desc(&MonitorHandle, INA219_ADDR_GND_GND, I2C_PORT, I2C_SDA, I2C_SCL);
	ina219_init(&MonitorHandle);
	ina219_configure(&MonitorHandle, INA219_BUS_RANGE_32V, INA219_GAIN_0_125, INA219_RES_12BIT_1S, INA219_RES_12BIT_1S, INA219_MODE_CONT_SHUNT_BUS);
	ina219_calibrate(&MonitorHandle, SHUNT_RESISTANCE);

	//init timer
	esp_timer_init();

	// assign unique node id
	Unique_NodeID = 101; // place holder value

	// init variables
	Sending = false;
	Response = false;
	Period = DEFAULT_PERIOD;

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
	
	int IterationCount = 0;
	while(1) {
		// Avoid watchdog
		vTaskDelay(1);

		// check incoming packets
		if (MainPacket_Ready) {
			GetPacket();
			ParsePacket();
		}

		// Stay awake for some time before sleep
		if (IterationCount++ < 20000) {
			continue;
		}

		// if some time has passed: 
		// Go to sleep for period
		esp_sleep_enable_timer_wakeup(Period * MICROSECOND_TO_SECOND);
		esp_deep_sleep_start();
	}
}