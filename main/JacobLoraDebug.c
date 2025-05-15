/**
 * @file LoraLibTest.c
 * @author Jacob Dennon (jdennon@ucsc.edu)
 * @brief Test harness for the LoRa library
 * @version 0.1
 * @date 2025-04-16
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "../../include/LoRa_main.h"
#include "driver/gpio.h"
#include "esp_log.h"

#define tx_timeout 9000
#define MIRROR_MOSI 40

static const char *TAG = "JacobLoraDebug.c";
static sx1262_handle_t LoRa;
static uint8_t test_buf[5] = "Test";

void app_main(void)
{
	ESP_LOGI(TAG, "before lora begin");
	sx1262_lora_begin(&LoRa);
	// ESP_LOGI(TAG, "past lora begin before lora task");
	// init_lora_task();
	// ESP_LOGI(TAG, "past init lora task, before gpioinit");
	// sx1262_interface_dio1_gpio_init(&LoRa);
	// ESP_LOGI(TAG, "Past gpio init before set send");

	// if (sx1262_lora_set_send_mode(&LoRa))
	// {
	// 	ESP_LOGI(TAG, "Set send failed");
	// 	sx1262_interface_dio1_gpio_deinit();
	// 	sx1262_lora_deinit(&LoRa);
	// }

	// ESP_LOGI(TAG, "Beginning send command");
	// if (sx1262_lora_send(&LoRa, test_buf, sizeof(test_buf)))
	// {
	// 	ESP_LOGE(TAG, "Send failed");
	// 	sx1262_interface_dio1_gpio_deinit();
	// 	sx1262_lora_deinit(&LoRa);
	// }
	// else
	// {
	// 	sx1262_interface_debug_print("\n");
	// 	sx1262_interface_dio1_gpio_deinit();
	// 	sx1262_lora_deinit(&LoRa);
	// }

	while (1);
}