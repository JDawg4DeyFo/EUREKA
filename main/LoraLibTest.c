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
   esp32_SPI_bus_init();
   esp32_SPI_WRITE_READ_test();
   esp32_SPI_bus_deinit();

   while(1);
}