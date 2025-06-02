#include <stdio.h>
#include <inttypes.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#include "../include/LoRa.h"

#include <ina219.h>

static const char *TAG = "Crap.c";

#define PLACEHOLDER_UNIQUEID 102
// Voltage monitor defines
#define SHUNT_RESISTANCE 0.24
#define CRITICAL_VOLTAGE 11.0 // ACCURATE VALUE NEEDED ... minimum voltage is 10V for battery

// i2c defines NOTE: probably should be replaced with CONFIG_I2C values
#define I2C_SCL 42
#define I2C_SDA 41
#define I2C_PORT 0

#include "../include/Protocol.h"

static uint32_t TempTimestamp;
static ina219_t MonitorHandle;
uint8_t tx_len;

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

bool SendDebugPacket()
{
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

	ESP_LOGI(TAG, "Debug packet function reached");
	if (LoRaSend(buffer, tx_len, SX126x_TXMODE_SYNC) == false)
	{
		ESP_LOGE(TAG, "LoRaSend fail");
	}

	int lost = GetPacketLost();
	if (lost != 0)
	{
		ESP_LOGW(pcTaskGetName(NULL), "%d packets lost", lost);
	}

	vTaskDelay(pdMS_TO_TICKS(1000));

	return true;
}

void app_main()
{
	// Initialize ina
	// Power_init();
	ina219_init_desc(&MonitorHandle, INA219_ADDR_GND_GND, I2C_PORT, I2C_SDA, I2C_SCL);
	ina219_init(&MonitorHandle);
	ina219_configure(&MonitorHandle, INA219_BUS_RANGE_32V, INA219_GAIN_0_125, INA219_RES_12BIT_1S, INA219_RES_12BIT_1S, INA219_MODE_CONT_SHUNT_BUS);
	ina219_calibrate(&MonitorHandle, SHUNT_RESISTANCE);

	// Iniitalize lora
	LoRaInit();
	int8_t txPowerInDbm = 22;

	uint32_t frequencyInHz = 915000000;

	float txcoVoltage = 3.3;
	bool useRegulatorLDO = true;

	// Begin the lora module
	if (LoRaBegin(frequencyInHz, txPowerInDbm, txcoVoltage, useRegulatorLDO) != 0) {
		ESP_LOGE(TAG, "Does not recognize the module");
		while(1) {
			vTaskDelay(1);
		}
	}
	
	// config values
	uint8_t spreadingFactor = 12;
	uint8_t bandwidth = 4;
	uint8_t codingRate = 1;
	uint16_t preambleLength = 8;
	uint8_t payloadLen = 0;
	bool crcOn = true;
	bool invertIrq = false;

	LoRaConfig(spreadingFactor, bandwidth, codingRate, preambleLength, payloadLen, crcOn, invertIrq);

	while(1) {
		SendDebugPacket();
	}

}