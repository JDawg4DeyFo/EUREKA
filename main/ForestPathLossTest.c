/**
 * @file ForestPathLossTest.c
 * @author Edouard Valenzuela (ecvalenz@ucsc.edu)
 * @brief Code to conduct the experiment that measures the path loss in a forested environment
 * @version 0.1
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

#include "../include/LoRa.h"

static const char *TAG = "MAIN";

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

#if CONFIG_PRIMARY
void task_primary(void *pvParameters)
{
    ESP_LOGI(pcTaskGetName(NULL), "Start");
	uint8_t txData[256]; // Maximum Payload size of SX1261/62/68 is 255
	uint8_t rxData[256]; // Maximum Payload size of SX1261/62/68 is 255
	while(1) {
		uint8_t rxLen = LoRaReceive(rxData, sizeof(rxData));
		if ( rxLen > 0 ) { 
			printf("Receive rxLen:%d\n", rxLen);
			for(int i=0;i< rxLen;i++) {
				printf("%02x ",rxData[i]);
			}
			printf("\n");

			for(int i=0;i< rxLen;i++) {
				if (rxData[i] > 0x19 && rxData[i] < 0x7F) {
					char myChar = rxData[i];
					printf("%c", myChar);
				} else {
					printf("?");
				}
			}
			printf("\n");

			int8_t rssi, snr;
			GetPacketStatus(&rssi, &snr);
			printf("rssi=%d[dBm] snr=%d[dB]\n", rssi, snr);

			for(int i=0;i<rxLen;i++) {
				if (isupper(rxData[i])) {
					txData[i] = tolower(rxData[i]);
				} else {
					txData[i] = toupper(rxData[i]);
				}
			}

			// Wait for transmission to complete
			if (LoRaSend(txData, rxLen, SX126x_TXMODE_SYNC)) {
				ESP_LOGD(pcTaskGetName(NULL), "Send success");
			} else {
				ESP_LOGE(pcTaskGetName(NULL), "LoRaSend fail");
			}

		}
		vTaskDelay(1); // Avoid WatchDog alerts
	} // end while
}


#if CONFIG_SECONDARY
void task_secondary(void *pvParameters)
{
    ESP_LOGI(pcTaskGetName(NULL), "Start");
	uint8_t txData[256]; // Maximum Payload size of SX1261/62/68 is 255
	uint8_t rxData[256]; // Maximum Payload size of SX1261/62/68 is 255
	while(1) {
		TickType_t nowTick = xTaskGetTickCount();
		int txLen = sprintf((char *)txData, "Hello World %"PRIu32, nowTick);
		//uint8_t len = strlen((char *)txData);

		// Wait for transmission to complete
		if (LoRaSend(txData, txLen, SX126x_TXMODE_SYNC)) {
			//ESP_LOGI(pcTaskGetName(NULL), "Send success");

			bool waiting = true;
			TickType_t startTick = xTaskGetTickCount();
			while(waiting) {
				uint8_t rxLen = LoRaReceive(rxData, sizeof(rxData));
				TickType_t currentTick = xTaskGetTickCount();
				TickType_t diffTick = currentTick - startTick;
				if ( rxLen > 0 ) {
					ESP_LOGI(pcTaskGetName(NULL), "%d byte packet received:[%.*s]", rxLen, rxLen, rxData);
					ESP_LOGI(pcTaskGetName(NULL), "Response time is %"PRIu32" millisecond", diffTick * portTICK_PERIOD_MS);
					waiting = false;
				}
				
				ESP_LOGD(pcTaskGetName(NULL), "diffTick=%"PRIu32, diffTick);
				if (diffTick > TIMEOUT) {
					ESP_LOGW(pcTaskGetName(NULL), "No response within %d ticks", TIMEOUT);
					waiting = false;
				}
				vTaskDelay(1); // Avoid WatchDog alerts
			} // end waiting

		} else {
			ESP_LOGE(pcTaskGetName(NULL), "Send fail");
		}

		vTaskDelay(pdMS_TO_TICKS(1000));
	} // end while
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

    printf("Select Bandwidth (enter number): ");
    scanf("%hhu", &bandwidth_selection);
    if (bandwidth_selection < 1 || bandwidth_selection > bwCount) {
        printf("Invalid bandwidth selection.\n");
        return 1;
    }

    printf("\nAvailable Spreading Factor Options:\n");
    for (int i = 0; i < sfCount; i++) {
        printf("  %d: SF%d\n", i + 1, SF_Options[i]);
    }

    printf("Select Spreading Factor (enter number): ");
    scanf("%hhu", &spreadingFactor_selection);
    if (spreadingFactor_selection < 1 || spreadingFactor_selection > sfCount) {
        printf("Invalid spreading factor selection.\n");
        return 1;
    }

    // Cast enum values into uint8_t
    bandwidth = (uint8_t)BW_Options[bandwidth_selection - 1];
    spreadingFactor = (uint8_t)SF_Options[spreadingFactor_selection - 1];

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

#if CONFIG_PRIMARY
	xTaskCreate(&task_primary, "PRIMARY", 1024*4, NULL, 5, NULL);
#endif
#if CONFIG_SECONDARY
	xTaskCreate(&task_secondary, "SECONDARY", 1024*4, NULL, 5, NULL);
#endif
}
