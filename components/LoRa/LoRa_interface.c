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
#include "LoRa_interface.h"

#define GPIO_MOSI 6
#define GPIO_MISO 3
#define GPIO_SCK 5
#define GPIO_CS 7

static sx1262_handle_t LoRa_handle;
static spi_device_handle_t slave_handle; 


  /**
 * @brief  interface the spi bus with the sx1262 and add the device
 * @return status code
 *         - 0 success
 *         - 1 spi init failed
 * @note   none
 */

 uint8_t esp32_SPI_bus_init(void){

   spi_common_dma_t DMA_channel = SPI_DMA_CH_AUTO;
   spi_host_device_t spi_bus1 = SPI_HOST;

   const spi_bus_config_t bus_pins = {
      .miso_io_num = GPIO_MISO,
      .mosi_io_num = GPIO_MOSI,
      .sclk_io_num = GPIO_SCK,
      .max_transfer_sz = 32,
      .quadwp_io_num = -1
      .quadwp_hd_num = -1;
   };

   spi_device_interface_config_t dev_config = {
      .command_bits = 0,
      .address_bits = 0,
      .dummy_bits = 0,     
      .mode = 0,
      .clock_speed_hz = SPI_MASTER_FREQ_8M, //clk_speed = 8 MHz
      .duty_cycle_pos = 128, //Duty cycle = 50%
      .spics_io_num = GPIO_CS,
      .queue_size = 1,
      .cs_ena_posttrans = 1,
   };

   if (spi_bus_initialize(spi_bus1, &bus_pins, DMA_channel) == ESP_OK){
      printf("spi_bus_initalize is a success\n");
      if (spi_bus_add_device(spi_bus1, &dev_config, &slave_handle) == ESP_OK){
         printf("spi_bus_add_device is a success\n");
         sx1262_init(&LoRa_handle);
      } else{
         printf("spi_bus_add_device failed\n");
         return 1;
      }
   } else {
      printf("spi_bus_initalize failed\n");
      return 1;
   }
   return 0;
 }

 /**
 * @brief  free the spi bus from the sx1262 but first remove the device to do so
 * @return status code
 *         - 0 success
 *         - 1 spi deinit failed
 * @note   none
 */
uint8_t esp32_SPI_bus_deinit(void){
   
}

 /**
 * @brief  Read and write via SPI one transaction 
 * @return status code
 *         - 0 success
 *         - 1 spi read and write asynch failed
 * @note   none
 */
uint8_t esp32_SPI_READ_WRITE_synch(void){
   spi_transaction_t transaction_mes;
   memset(&transaction_mes, 0, sizeof(transaction_mes));

   spi_device_transmit(slave_handle, &transaction_mes);
}

uint8_t main(void){
   return 0;
}
