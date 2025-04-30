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

#include "../include/LoRa_main.h"
#define test_one_sec_delay 1e6

const double test_delay = test_one_sec_delay;
static sx1262_handle_t LoRa_1;
void app_main(void){
  sx1262_lora_begin(&LoRa_1);
  sx1262_lora_set_continuous_transmit_mode(&LoRa_1);
  sx1262_lora_irq_handler(&LoRa_1);
  //sx1262_lora_deinit(&LoRa_1);
   //while(1);
 }