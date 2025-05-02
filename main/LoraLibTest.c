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

#include "../../include/driver_sx1262_send_receive_test.h"
#include "../../include/driver_sx1262_cad_test.h"
#include "../../include/LoRa_main.h"

static sx1262_handle_t LoRa;
//static const uint32_t rx_timeout = 10;
void app_main(void){
  sx1262_lora_begin(&LoRa);
  sx1262_lora_send(&LoRa, (uint8_t *)"123", strlen("123"));
  //sx1262_send_test();
  //sx1262_receive_test(rx_timeout);
 }