/**
 * @file LoRa_interface.c
 * @author Edouard Valenzuela (ecvalenz@ucsc.edu)
 * @brief Library for interfacing with the ESP32 with the SX1262
 * @version 1.0
 * @date 2025-04-14
 * 
 * @copyright Copyright (c) 2025
 * 
 * 
 *  
 */
#include "../../include/LoRa_interface.h"

#define GPIO_MOSI 6
#define GPIO_MISO 3
#define GPIO_SCK 5
#define GPIO_CS 7

static sx1262_handle_t LoRa_handle;
static spi_device_handle_t slave_handle; 


spi_common_dma_t DMA_channel = SPI_DMA_CH_AUTO;
spi_host_device_t spi_bus1 = SPI_HOST;

const spi_bus_config_t bus_pins = {
   .miso_io_num = GPIO_MISO,
   .mosi_io_num = GPIO_MOSI,
   .sclk_io_num = GPIO_SCK,
   .max_transfer_sz = 32,
   .quadwp_io_num = -1,
   .quadwp_hd_num = -1,
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

  /**
 * @brief  interface the spi bus with the sx1262 and add the device
 * @return status code
 *         - 0 success
 *         - 1 spi init failed
 * @note   none
 */

 esp_err_t esp32_SPI_bus_init(void){

   spi_common_dma_t DMA_channel = SPI_DMA_CH_AUTO;

   esp_err_t check_result = spi_bus_initialize(spi_bus1, &bus_pins, DMA_channel);

   if (check_result  != ESP_OK){
      printf("spi_bus_initalize failed\n");
      return check_result;
   } 

   printf("spi_bus_initalize is a success\n");

   check_result = spi_bus_add_device(spi_bus1, &dev_config, &slave_handle);

   if (check_result != ESP_OK){
      printf("spi_bus_add_device failed\n");
      return check_result;
   }
   printf("spi_bus_add_device is a success\n");

   uint8_t check_LoRa_init = sx1262_init(&LoRa_handle);

   if (check_LoRa_init != 0){
      printf("LoRa chip failed to initialize, reason: %d\n", sx1262_init(&LoRa_handle));
      return ESP_ERR_NOT_FOUND;
   } 
   printf("LoRa chip successfully initaliazed\n");
   printf("Successful initialization\n");

   return ESP_OK;
}
 

 /**
 * @brief  free the spi bus from the sx1262 but first remove the device to do so
 * @return status code
 *         - 0 success
 *         - 1 spi deinit failed
 * @note   none
 */
esp_err_t esp32_SPI_bus_deinit(void){

   uint8_t check_LoRa_deinit = sx1262_deinit(&LoRa_handle);

   if (check_LoRa_deinit != 1){
      printf("LoRa chip failed to deinitialize, reason: %u\n", sx1262_deinit(&LoRa_handle));
      return ESP_ERR_NOT_FOUND;
   }
   printf("LoRa chip successfully deinitaliazed\n");

   esp_err_t check_result2 = spi_bus_remove_device(slave_handle);

   if (check_result2 != ESP_OK){
      return check_result2;
   } 

   printf("spi_bus_remove_device is a success\n");

   check_result2 = spi_bus_free(spi_bus1);
   if (check_result2 != ESP_OK){
      printf("spi_bus_add_device failed\n");
      return check_result2;
   } 

   printf("spi__bus_free is a success\n");
   printf("Successful deinitialization!\n");

   return ESP_OK;
}

 /**
 * @brief  Read and write via SPI one transaction 
 * @return status code
 *         - 0 success
 *         - 1 spi read and write asynch failed
 * @note   none
 */
esp_err_t esp32_SPI_WRITE_READ_test(void){

   spi_transaction_t transaction_mes{
      .txdata = {"test"},
      .length = 32,
      .rxlength = 32,
      .user = "htx",
      .flags = SPI_TRANS_USE_TXDATA | SPI_TRANS_USE_RXDATA,
   };

   esp_err_t check_result3 = spi_device_transmit(slave_handle, &transaction_mes);
   if (check_result3 != ESP_OK){
      printf("spi_device_transmit failed\n");
      return check_result3;
   } 

   printf("spi_device_transmit is a success\n");
   printf("Successful single transaction, rx_data = ");
   for (int i = 0; i < sizeof(transaction_mes.rx_data); i++){
      printf("%c", transaction_mes.rx_data[i]);
   }
   printf("\n");

   return ESP_OK;
   
}
