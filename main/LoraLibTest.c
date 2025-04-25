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


void app_main(void){
   sx1262_device_init();
   esp32_SPI_bus_deinit();
   sx1262_device_deinit();
   

   while(1);
 }