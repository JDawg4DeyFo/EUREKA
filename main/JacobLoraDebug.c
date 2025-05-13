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

#include "../../include/LoRa_main.h"
#include "driver/gpio.h"
#include "../../include/driver_sx1262_cad_test.h"
#include "soc/gpio_sig_map.h"
#include "hal/gpio_types.h"

#define tx_timeout 9000
#define MIRROR_MISO 41
#define MIRROR_MOSI 40


static sx1262_handle_t LoRa;
static uint8_t test_buf[5] = "Test";

// Route SPI pins to other, accessible pins on the ESP32.
void mirror_spi2();
{
	// Mirror MISO
	gpio_reset_pin(MIRROR_MISO);
	gpio_set_direction(MIRROR_MISO, GPIO_MODE_OUTPUT);
	gpio_matrix_out(MIRROR_MISO, SPI2_Q_IDX, false, false);
	
	// Mirror MOSI
	gpio_reset_pin(MIRROR_MOSI);
	gpio_set_direction(MIRROR_MOSI, GPIO_MODE_OUTPUT);
	gpio_matrix_out(MIRROR_MOSI, FSPI_D_OUT_IDX, false, false);
}

void app_main(void){
  sx1262_lora_begin(&LoRa);
  init_lora_task();
  sx1262_interface_dio1_gpio_init(&LoRa);

  
  if (sx1262_lora_set_send_mode(&LoRa))
  {
    sx1262_interface_debug_print("Set send mode failed\n");
    sx1262_interface_dio1_gpio_deinit();
    sx1262_lora_deinit(&LoRa);
  } 


  if(sx1262_lora_send(&LoRa, test_buf, sizeof(test_buf))){
    sx1262_interface_dio1_gpio_deinit();
    sx1262_lora_deinit(&LoRa);
  }else{
    sx1262_interface_debug_print("Transmission is a success\n");
    sx1262_interface_dio1_gpio_deinit();
    sx1262_lora_deinit(&LoRa);
  }
  
  while(1);
  
}