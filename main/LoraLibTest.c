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

#define tx_timeout 9000
#define GPIO_38 38
static sx1262_handle_t LoRa;
static uint8_t test_buf[5] = "Test";

const gpio_config_t GPIO_TEST_38 = {
  .pin_bit_mask = 1ULL << GPIO_38,        
  .mode = GPIO_MODE_DEF_OUTPUT,               
  .pull_up_en = GPIO_PULLUP_DISABLE ,       
  .pull_down_en = GPIO_PULLDOWN_DISABLE ,   
  .intr_type = GPIO_INTR_DISABLE,
};



void app_main(void){
  gpio_config(&GPIO_TEST_38);
  gpio_set_level(GPIO_38, 1);
  sx1262_lora_begin(&LoRa);
  sx1262_interface_dio1_gpio_init(&LoRa);

  /*
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
  */
  while(1);
  
}