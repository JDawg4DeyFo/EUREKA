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
#define GPIO_BUSY 34
#define GPIO_RESET 8

static sx1262_handle_t LoRa_handle;
static spi_device_handle_t slave_handle; 


spi_common_dma_t DMA_channel = SPI_DMA_CH_AUTO;
spi_host_device_t spi_bus1 = SPI3_HOST;

const spi_bus_config_t bus_pins = {
   .miso_io_num = GPIO_MISO,
   .mosi_io_num = GPIO_MOSI,
   .sclk_io_num = GPIO_SCK,
   .max_transfer_sz = 32,
   .quadwp_io_num = -1,
   .quadhd_io_num = -1,
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

spi_transaction_t transaction_mes = {
   .tx_data = {"test"},
   .length = 32,
   .rxlength = 32,
   .user = "htx",
   .flags = SPI_TRANS_USE_TXDATA | SPI_TRANS_USE_RXDATA,
};

  /**
 * @brief  interface the spi bus with the sx1262 and add the device
 * @return status code
 *         - 0 success
 *         - 1 spi init failed
 * @note   none
 */

 uint8_t esp32_SPI_bus_init(void){

   spi_common_dma_t DMA_channel = SPI_DMA_CH_AUTO;

   esp_err_t check_result = spi_bus_initialize(spi_bus1, &bus_pins, DMA_channel);

   if (check_result  != ESP_OK){
      printf("spi_bus_initalize failed due to: %d\n", check_result);
      return 1;
   } 

   printf("spi_bus_initalize is a success\n");

   check_result = spi_bus_add_device(spi_bus1, &dev_config, &slave_handle);

   if (check_result != ESP_OK){
      printf("spi_bus_add_device failed due to: %d\n", check_result);
      return 1;
   }
   printf("spi_bus_add_device is a success\n");

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

   uint8_t check_LoRa_deinit = sx1262_deinit(&LoRa_handle);

   if (check_LoRa_deinit != 1){
      printf("LoRa chip failed to deinitialize, reason: %u\n", sx1262_deinit(&LoRa_handle));
      return 1;
   }
   printf("LoRa chip successfully deinitaliazed\n");

   esp_err_t check_result2 = spi_bus_remove_device(slave_handle);

   if (check_result2 != ESP_OK){
      printf("spi_bus_add_device failed due to: %d\n", check_result2);
      return 1;
   } 

   printf("spi_bus_remove_device is a success\n");

   check_result2 = spi_bus_free(spi_bus1);
   if (check_result2 != ESP_OK){
      printf("spi_bus_add_device failed due to: %d\n", check_result2);
      return 1;
   } 

   printf("spi_bus_free is a success\n");
   printf("Successful deinitialization!\n");

   return 0;
}

 /**
 * @brief  Read and write via SPI one transaction 
 * @return status code
 *         - 0 success
 *         - 1 spi read and write asynch failed
 * @note   none
 */
uint8_t esp32_SPI_WRITE_READ_test(void){

   esp_err_t check_result3 = spi_device_transmit(slave_handle, &transaction_mes);
   if (check_result3 != ESP_OK){
      printf("spi_device_transmit failed due to: %d\n", check_result3);
      return 1;
   } 

   printf("spi_device_transmit is a success\n");
   printf("Successful single transaction, rx_data = ");

   for (int i = 0; i < sizeof(transaction_mes.rx_data); i++){
      printf("%c", transaction_mes.rx_data[i]);
   }
   printf("\n");

   return 0;
   
}

 /**
 * @brief  Link the SPI bus the sx1262 and the ESP32 with respect to the ESP32 
 * @return status code
 *         - 1 success
 *         - 0 LoRa chip failed to initialize
 * @note   none
 */

uint8_t sx1262_device_init(void){
   DRIVER_SX1262_LINK_INIT(&LoRa_handle, sx1262_handle_t);
   DRIVER_SX1262_LINK_SPI_INIT(&LoRa_handle, esp32_SPI_bus_init);
   DRIVER_SX1262_LINK_SPI_DEINIT(&LoRa_handle, esp32_SPI_bus_deinit);
   DRIVER_SX1262_LINK_SPI_WRITE_READ(&LoRa_handle, esp32_SPI_WRITE_READ_test);
   DRIVER_SX1262_LINK_RESET_GPIO_INIT(&LoRa_handle, );
   DRIVER_SX1262_LINK_RESET_GPIO_DEINIT(&LoRa_handle, );
   DRIVER_SX1262_LINK_RESET_GPIO_WRITE(&LoRa_handle, );
   DRIVER_SX1262_LINK_BUSY_GPIO_INIT(&LoRa_handle, );
   DRIVER_SX1262_LINK_BUSY_GPIO_DEINIT(&LoRa_handle, );
   DRIVER_SX1262_LINK_BUSY_GPIO_READ(&LoRa_handle, );
   DRIVER_SX1262_LINK_DELAY_MS(&LoRa_handle, );
   DRIVER_SX1262_LINK_DEBUG_PRINT(&LoRa_handle, );
   DRIVER_SX1262_LINK_RECEIVE_CALLBACK(&LoRa_handle, );
   
   uint8_t check_LoRa_init = sx1262_init(&LoRa_handle);

   if (check_LoRa_init != 0){
      printf("LoRa chip failed to initialize, reason: %d\n", sx1262_init(&LoRa_handle));
      return 1;
   } 

   printf("LoRa chip successfully initaliazed\n");
   return 0;

}

 /**
 * @brief  Deinitalize the LoRa
 * @return status code
 *         - 1 success
 *         - 0 LoRa chip failed to deinitialize
 * @note   none
 */

uint8_t sx1262_device_deinit(void){
   if (sx1262_deinit(&LoRa_handle) != 0) {
      printf("LoRa chip failed to deinitialize");
      return 1;
   }

   printf("LoRa chip successfully deinitaliazed\n");
   return 0;
}
