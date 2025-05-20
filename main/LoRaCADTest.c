/* The example of ESP-IDF
 *
 * This sample code is in the public domain.
 */

// from: https://github.com/nopnop2002/esp-idf-sx126x/blob/main/basic/main/main.c

#include <stdio.h>
#include <inttypes.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#include "../include/LoRa.h"

#define cadDetMin 10
#define cadDetPeakSF7_8 22
#define cadDetPeakSF9 23
#define cadDetPeakSF10 24
#define cadDetPeakSF11 25
#define cadDetPeakSF12 28
#define cadTimeout  10

static const char *TAG = "MAIN";
static uint8_t spreadingFactor;

void task_CAD(void *pvParameters)
{
    //Start CAD Test with the signal bandwidth equal to 125 kHz
    ESP_LOGI(TAG, "Entering CAD test");
    uint8_t cadDetPeak = 0;
    uint8_t cadSymbNum = 0;

    switch(spreadingFactor){
        case 7:
            cadSymbNum = SX126X_CAD_ON_2_SYMB ;
            cadDetPeak = cadDetPeakSF7_8;
            break;
        case 8:
            cadSymbNum = SX126X_CAD_ON_2_SYMB ;
            cadDetPeak = cadDetPeakSF7_8;
            break;
        case 9:
            cadSymbNum = SX126X_CAD_ON_4_SYMB;
            cadDetPeak = cadDetPeakSF9;
            break;
        case 10:
            cadSymbNum = SX126X_CAD_ON_4_SYMB;
            cadDetPeak = cadDetPeakSF10;
            break;
        case 11:
            cadSymbNum = SX126X_CAD_ON_4_SYMB;
            cadDetPeak = cadDetPeakSF11;
            break;
        case 12:
            cadSymbNum = SX126X_CAD_ON_4_SYMB;
            cadDetPeak = cadDetPeakSF12;
            break;
    }

    SetCadParams(cadSymbNum, cadDetPeak, cadDetMin, SX126X_CAD_GOTO_STDBY, cadTimeout);
    SetCad();

    while(1){
        uint16_t irqRegs = GetIrqStatus();
        if(irqRegs & SX126X_IRQ_CAD_DETECTED)
        {
            ClearIrqStatus(SX126X_IRQ_ALL);
            ESP_LOGI(TAG, "CAD Detected");
            break;
        }

        if(irqRegs & SX126X_IRQ_CAD_DONE)
        {
            ClearIrqStatus(SX126X_IRQ_ALL);
            ESP_LOGI(TAG, "No CAD detected, CAD Done");
            break;
        }
    }


    vTaskDelay(pdMS_TO_TICKS(10));  // small delay to avoid busy loop

    ESP_LOGI(pcTaskGetName(NULL), "CAD Test done, stopping task.");

    vTaskDelete(NULL); // Delete self to stop task
	
}

void app_main()
{
	// Initialize LoRa
	LoRaInit();
	int8_t txPowerInDbm = 22;

	uint32_t frequencyInHz = 0;
#if CONFIG_911MHZ
	frequencyInHz = 911000000;
	ESP_LOGI(TAG, "Frequency is 911MHz");
#elif CONFIG_908MHZ
	frequencyInHz = 908000000;
	ESP_LOGI(TAG, "Frequency is 908MHz");
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
	float tcxoVoltage = 0.0; // don't use TCXO
	bool useRegulatorLDO = false; // use only LDO in all modes
#endif

	//LoRaDebugPrint(true);
	if (LoRaBegin(frequencyInHz, txPowerInDbm, tcxoVoltage, useRegulatorLDO) != 0) {
		ESP_LOGE(TAG, "Does not recognize the module");
		while(1) {
			vTaskDelay(1);
		}
	}
	
	spreadingFactor = 12;
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
	xTaskCreate(&task_CAD, "CAD Test", 1024*4, NULL, 5, NULL);
}
