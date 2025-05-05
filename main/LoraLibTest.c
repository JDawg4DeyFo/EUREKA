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
#include "../../include/driver_sx1262_cad_test.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#define tx_timeout 9000

static sx1262_handle_t LoRa;
static uint8_t test_buf[5] = "Test";

static const char *TAG = "LoraLibTest.c";

void app_main(void){
	ESP_LOGI(TAG, "Testing lora begin");
  sx1262_lora_begin(&LoRa);
	ESP_LOGI(TAG, "Testing dio1 gpio init");
  sx1262_interface_dio1_gpio_init(&LoRa);

	ESP_LOGI(TAG, "Testing shot receive mode");
  if (sx1262_lora_set_shot_receive_mode(&LoRa, tx_timeout)){
    sx1262_interface_debug_print("Set RX failed\n");
  
    sx1262_interface_dio1_gpio_deinit();
    sx1262_lora_deinit(&LoRa);

  } else {
    sx1262_interface_delay_ms(tx_timeout);
    sx1262_interface_debug_print("Set RX works\n");
  }

  ESP_LOGI(TAG, "Writing buffer");
  sx1262_write_buffer(&LoRa, 0x00, test_buf, sizeof(test_buf));
  /* set lora packet params */
  sx1262_set_lora_packet_params(&LoRa, SX1262_LORA_DEFAULT_PREAMBLE_LENGTH,SX1262_LORA_DEFAULT_HEADER, SX1262_LORA_DEFAULT_BUFFER_SIZE,
  SX1262_LORA_DEFAULT_CRC_TYPE, SX1262_LORA_DEFAULT_INVERT_IQ);
  sx1262_lora_set_send_mode(&LoRa);

	ESP_LOGI(TAG, "Testing set_Tx");
  if (sx1262_set_tx(&LoRa, tx_timeout)){
    sx1262_interface_debug_print("Transmitting a packet failed\n");
  
    sx1262_interface_dio1_gpio_deinit();
    sx1262_lora_deinit(&LoRa);

  } else {
    sx1262_interface_delay_ms(tx_timeout);
    sx1262_interface_debug_print("Transmitting a packet works\n");

  }

	ESP_LOGI(TAG, "Testing continous wave");
  if(sx1262_set_tx_continuous_wave(&LoRa)){
    sx1262_interface_debug_print("TX Continuous fails\n");
    sx1262_interface_dio1_gpio_deinit();
    sx1262_lora_deinit(&LoRa);
  }else {
    sx1262_interface_debug_print("TX Continuous works\n");
    sx1262_interface_debug_print("Test Done\n");
    sx1262_interface_dio1_gpio_deinit();
    sx1262_lora_deinit(&LoRa);
    
  }

	ESP_LOGI(TAG, "About to exit program");
}
