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

static sx1262_handle_t LoRa_1;
uint8_t enable;

void app_main(void){

  const char *test_msg = "Test";
  uint8_t test_buf[15];
  uint16_t test_buf_len;
  
  strcpy((char *)test_buf, test_msg);
  test_buf_len = sizeof(test_buf);

  sx1262_lora_begin(&LoRa_1);
  sx1262_set_dio_output_enable(&LoRa_1, enable);
  sx1262_lora_set_send_mode(&LoRa_1);
  sx1262_lora_send(&LoRa_1, test_buf, test_buf_len);
  sx1262_lora_deinit(&LoRa_1);
   //while(1);
 }