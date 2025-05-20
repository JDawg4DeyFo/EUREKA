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
} LoRa_SpreadingFactor

#if CONFIG_PRIMARY
void task_primary(void *pvParameters)
{

}


#if CONFIG_SECONDARY
void task_secondary(void *pvParameters)
{

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

    LoRa_SpreadingFactor SF_Options[] = {SF_5, SF_6, SF_7, SF_8, SF_9, SF_10, SF_11, SF_12};

	uint8_t spreadingFactor, bandwidth;


    printf("Available Bandwidth Options:\n");
    for (int i = 0; i < sizeof(BW_Options) / sizeof(BW_Options[0]); i++) {
        printf("  %d: %d kHz\n", i + 1, BW_Options[i]);
    }

    printf("Select Bandwidth (enter number): ");
    scanf("%d", &bandwidth);
    if (bandwidth < 1 || bandwidth > 3) {
        printf("Invalid bandwidth selection.\n");
        return 1;
    }

    printf("\nAvailable Spreading Factor Options:\n");
    for (int i = 0; i < sizeof(SF_Options) / sizeof(SF_Options[0]); i++) {
        printf("  %d: SF%d\n", i + 1, SF_Options[i]);
    }

    printf("Select Spreading Factor (enter number): ");
    scanf("%d", &spreadingFactor);
    if (spreadingFactor < 1 || spreadingFactor > 6) {
        printf("Invalid spreading factor selection.\n");
        return 1;
    }

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
