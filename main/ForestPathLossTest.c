/**
 * @file ForestPathLossTest.c
 * @author Edouard Valenzuela (ecvalenz@ucsc.edu)
 * @brief Code to conduct the experiment that measures the path loss in a forested environment
 * @version 1.0
 * @date 2025-05-19
 *
 * @copyright Copyright (c) 2025
 *
 */

#include <stdio.h>
#include <inttypes.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/uart.h"

#include "../include/LoRa.h"

static const char *TAG = "MAIN";
static const uint8_t fallback_mode = SX126X_RX_TX_FALLBACK_MODE_STDBY_RC;

typedef enum
{
    BANDWIDTH_7P81_KHZ  = 0,        /**< 7.81 kHz */
    BANDWIDTH_15P63_KHZ = 1,        /**< 15.63 kHz */
    BANDWIDTH_31P25_KHZ = 2,        /**< 31.25 kHz */
    BANDWIDTH_62P50_KHZ = 3,        /**< 62.50 kHz */
    BANDWIDTH_125_KHZ   = 4,        /**< 125 kHz */
    BANDWIDTH_250_KHZ   = 5,        /**< 250 kHz */
    BANDWIDTH_500_KHZ   = 6,        /**< 500 kHz */
} LoRa_Bandwidth;

typedef enum
{
    SF_5 = 5,     // Spreading Factor of 5
    SF_6 = 6,     // Spreading Factor of 6
    SF_7 = 7,     // Spreading Factor of 7
    SF_8 = 8,     // Spreading Factor of 8
    SF_9 = 9,     // Spreading Factor of 9
    SF_10 = 10,   // Spreading Factor of 10
    SF_11 = 11,   // Spreading Factor of 11
    SF_12 = 12,   // Spreading Factor of 12
} LoRa_SpreadingFactor;

typedef enum 
{
	STANDBY = 0,
	TRANSMIT = 1,
	RECEIVE = 2,
} EXP_States;

EXP_States current_state = STANDBY;

void task_main(void *pvParameters)
{
	while(1){
		switch(current_state){
		case STANDBY:
			ESP_LOGI(TAG, "In STANDBY state");

			LoRa_Bandwidth BW_Options[] = {BANDWIDTH_7P81_KHZ, BANDWIDTH_15P63_KHZ, BANDWIDTH_31P25_KHZ, BANDWIDTH_62P50_KHZ, BANDWIDTH_125_KHZ,
			BANDWIDTH_250_KHZ, BANDWIDTH_500_KHZ};

			const char *BW_Labels[] = {
				"7.81 kHz", "15.63 kHz", "31.25 kHz", "62.50 kHz",
				"125 kHz", "250 kHz", "500 kHz"
			};

			LoRa_SpreadingFactor SF_Options[] = {SF_5, SF_6, SF_7, SF_8, SF_9, SF_10, SF_11, SF_12};

			int bwCount = sizeof(BW_Options) / sizeof(BW_Options[0]);
			int sfCount = sizeof(SF_Options) / sizeof(SF_Options[0]);

			uint8_t spreadingFactor_selection, bandwidth_selection;
			uint8_t spreadingFactor, bandwidth;

			printf("Available Bandwidth Options:\n");
			for (int i = 0; i < bwCount; i++) {
				printf("  %d: %s\n", i + 1, BW_Labels[i]);
			}

			int c;
			printf("Select Bandwidth (enter number): ");
			scanf("%hhu\n", &bandwidth_selection);
			vTaskDelay(pdMS_TO_TICKS(60000));
			if (bandwidth_selection < 1 || bandwidth_selection > bwCount) {
				ESP_LOGE(TAG, "Invalid bandwidth selection.\n");
				while ((c = getchar()) != '\n' && c != EOF);
				break;
			}

			printf("\nAvailable Spreading Factor Options:\n");
			for (int i = 0; i < sfCount; i++) {
				printf("  %d: SF%d\n", i + 1, SF_Options[i]);
			}

			printf("Select Spreading Factor (enter number): ");
			scanf("%hhu\n", &spreadingFactor_selection);
			vTaskDelay(pdMS_TO_TICKS(60000));
			if (spreadingFactor_selection < 1 || spreadingFactor_selection > sfCount) {
				ESP_LOGE(TAG, "Invalid spreading factor selection.\n");
				while ((c = getchar()) != '\n' && c != EOF);
				break;
			}

			// Cast enum values into uint8_t
			bandwidth = (uint8_t)BW_Options[bandwidth_selection - 1];
			spreadingFactor = (uint8_t)SF_Options[spreadingFactor_selection - 1];

			uint8_t codingRate = 1;
			uint16_t preambleLength = 8;
			uint8_t payloadLen = 0;
			bool crcOn = true;
			bool invertIrq = false;

			LoRaConfig(spreadingFactor, bandwidth, codingRate, preambleLength, payloadLen, crcOn, invertIrq);

			uint8_t user_state;
			printf("Select State (enter number: 1 for TX, 2 for RX): ");
			scanf("%hhu", &user_state);
			if (user_state < 1 || user_state > 2) 
			{
				ESP_LOGE(TAG, "Invalid State selection.\n");
				break;
			} else {
				current_state = (EXP_States)user_state;
			}

			if(current_state == TRANSMIT)
			{
				ESP_LOGI(TAG, "Transitioning to TRANSMIT State");
			}

			if(current_state == RECEIVE)
			{
				ESP_LOGI(TAG, "Transitioning to RECEIVE State");
			}
			break;
		case TRANSMIT:
			ESP_LOGI(TAG, "In TRANSMIT state");
			uint8_t txbuf[256]; // Maximum Payload size of SX1261/62/68 is 255
			TickType_t nowTick = xTaskGetTickCount();
			int txLen = sprintf((char *)txbuf, "Hello World %"PRIu32, nowTick);
			ESP_LOGI(pcTaskGetName(NULL), "%d byte packet sent...", txLen);

			// Wait for transmission to complete
			if (LoRaSend(txbuf, txLen, SX126x_TXMODE_SYNC) == false) {
				ESP_LOGE(pcTaskGetName(NULL),"LoRaSend fail");
			}

			// Do not wait for the transmission to be completed
			//LoRaSend(buf, txLen, SX126x_TXMODE_ASYNC );

			int lost = GetPacketLost();
			if (lost != 0) {
				ESP_LOGW(pcTaskGetName(NULL), "%d packets lost", lost);
			}

			vTaskDelay(pdMS_TO_TICKS(1000));
			current_state = STANDBY;
			break;
		case RECEIVE:
			ESP_LOGI(TAG, "In RECEIVE state");
			uint8_t rxbuf[256]; // Maximum Payload size of SX1261/62/68 is 255
			uint8_t rxLen = LoRaReceive(rxbuf, sizeof(rxbuf));
			if ( rxLen > 0 ) { 
				ESP_LOGI(pcTaskGetName(NULL), "%d byte packet received:[%.*s]", rxLen, rxLen, rxbuf);
				int8_t rssi, snr;
				GetPacketStatus(&rssi, &snr);
				ESP_LOGI(pcTaskGetName(NULL), "rssi=%d[dBm] snr=%d[dB]", rssi, snr);
			}
			vTaskDelay(1); // Avoid WatchDog alerts

			current_state = STANDBY;
			break;
		}
	}
}


void app_main()
{
	// Initialize LoRa
	LoRaInit();
	int8_t txPowerInDbm = 22;

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

#if CONFIG_USE_TCXO
	ESP_LOGW(TAG, "Enable TCXO");
	float tcxoVoltage = 3.3; // use TCXO
	bool useRegulatorLDO = true; // use DCDC + LDO
#else
	ESP_LOGW(TAG, "Disable TCXO");
	float tcxoVoltage = 0.0;  // don't use TCXO
	bool useRegulatorLDO = false;  // use only LDO in all modes
#endif

	//LoRaDebugPrint(true);
	if (LoRaBegin(frequencyInHz, txPowerInDbm, tcxoVoltage, useRegulatorLDO) != 0) {
		ESP_LOGE(TAG, "Does not recognize the module");
		while(1) {
			vTaskDelay(1);
		}
	}
	
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
	SetRxTxFallbackMode(fallback_mode);
	xTaskCreate(&task_main, "Task Main", 1024*4, NULL, 5, NULL);
}
