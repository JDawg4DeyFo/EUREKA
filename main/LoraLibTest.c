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
#include "../../include/LoRa_interface.h"
#include "../../include/driver_sx1262_cad_test.h"
#include "freertos/semphr.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#define tx_timeout 9000
 
static sx1262_handle_t LoRa;
static uint8_t test_buf[5] = "Test";


void app_main(void){

  sx1262_lora_begin(&LoRa);
  sx1262_interface_dio1_gpio_init(&LoRa); // Pass the address of the static handle
 
  if(sx1262_lora_set_send_mode(&LoRa)){
    ESP_LOGE("ERROR", "Set send mode failed.");
  }

  if(sx1262_lora_send(&LoRa, test_buf, sizeof(test_buf))){
    ESP_LOGE("ERROR", "transmit failed.");
  } else{
    ESP_LOGI("RESULT", "TX DONE");
    sx1262_interface_debug_print("Test Done\n");
    sx1262_interface_dio1_gpio_deinit();
    sx1262_lora_deinit(&LoRa); 
  }


}