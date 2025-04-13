/**
 * @file LoRa_interface.c
 * @author Edouard Valenzuela (ecvalenz@ucsc.edu)
 * @brief Library for interfacing with the ESP32 with the SX1262
 * @version 0.1
 * @date 2025-04-12
 * 
 * @copyright Copyright (c) 2025
 * 
 * 
 *  
 */
#include "../include/LoRa_interface.h"

static const spi_host_device_t spi_bus1 = SPI1_HOST; //SPI1 is the bus used to communicate to the SX1262
 
 spi_bus_config_t bus_pin {
    .miso_io_num = 3;
    .mosi_io_num = 6;
    .sclk_io_num = 5;
    .max_transfer_sz = 32;
    .quadwp_io_num = -1;
    .quadwp_hd_num = -1;
 };
