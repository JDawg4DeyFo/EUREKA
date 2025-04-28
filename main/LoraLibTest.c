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

#include "../include/LoRa_interface.h"

static sx1262_handle_t LoRa_test_handle;
void app_main(void){
   sx1262_device_init(&LoRa_test_handle);
   sx1262_device_deinit(&LoRa_test_handle);
   //while(1);
 }