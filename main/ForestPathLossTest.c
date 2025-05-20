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

static const char *TAG = "MAIN";

#if CONFIG_SENDER
void task_tx(void *pvParameters)
{
	ESP_LOGI(pcTaskGetName(NULL), "Start");
	uint8_t buf[256]; // Maximum Payload size of SX1261/62/68 is 255
	int packets_sent = 0;

	while(packets_sent < 10) {
		TickType_t nowTick = xTaskGetTickCount();
		int txLen = sprintf((char *)buf, "EUREKA! %"PRIu32, nowTick);
		ESP_LOGI(pcTaskGetName(NULL), "%d byte packet sent...", txLen);

		// Wait for transmission to complete
		if (LoRaSend(buf, txLen, SX126x_TXMODE_SYNC) == false) {
			ESP_LOGE(pcTaskGetName(NULL),"LoRaSend fail");
		}

		// Do not wait for the transmission to be completed
		//LoRaSend(buf, txLen, SX126x_TXMODE_ASYNC );

		int lost = GetPacketLost();
		if (lost != 0) {
			ESP_LOGW(pcTaskGetName(NULL), "%d packets lost", lost);
		}
		packets_sent++;
		vTaskDelay(pdMS_TO_TICKS(1000));
	} 
	ESP_LOGI(pcTaskGetName(NULL), "Sent %d samples, stopping task.", packets_sent);

    vTaskDelete(NULL); // Delete self to stop task
}
#endif // CONFIG_SENDER



#if CONFIG_RECEIVER
void task_rx(void *pvParameters)
{
	ESP_LOGI(pcTaskGetName(NULL), "Start");
	uint8_t buf[256]; // Maximum Payload size of SX1261/62/68 is 255
	int packets_received = 0;
	while(packets_received < 10) {
		uint8_t rxLen = LoRaReceive(buf, sizeof(buf));
		if ( rxLen > 0 ) { 
			ESP_LOGI(pcTaskGetName(NULL), "%d byte packet received:[%.*s]", rxLen, rxLen, buf);

			int8_t rssi, snr;
			GetPacketStatus(&rssi, &snr);
			ESP_LOGI(pcTaskGetName(NULL), "rssi=%d[dBm] snr=%d[dB]", rssi, snr);
		}
		packets_received++;
		vTaskDelay(1); // Avoid WatchDog alerts
	} // end while
	ESP_LOGI(pcTaskGetName(NULL), "Received samples %d samples, stopping task.", packets_received);

    vTaskDelete(NULL); // Delete self to stop task
}
#endif // CONFIG_RECEIVER

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

#if CONFIG_SENDER
	xTaskCreate(&task_tx, "TX", 1024*4, NULL, 5, NULL);
#endif
#if CONFIG_RECEIVER
	xTaskCreate(&task_rx, "RX", 1024*4, NULL, 5, NULL);
#endif
}
