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
#define tx_timeout 1000

uint8_t tx_base_addr = 0x00;
uint8_t rx_base_addr = 0x00;
uint16_t status;

static sx1262_handle_t LoRa;
static uint8_t test_buf[5] = "Test";

void app_main(void){
  sx1262_lora_begin(&LoRa);
  sx1262_set_buffer_base_address(&LoRa, tx_base_addr, rx_base_addr);
  sx1262_write_buffer(&LoRa, tx_base_addr, test_buf, sizeof(test_buf));

  sx1262_lora_set_send_mode(&LoRa);

  if (sx1262_set_tx(&LoRa, tx_timeout)){
    sx1262_interface_debug_print("Transmitting a packet failed\n");
    sx1262_lora_deinit(&LoRa);
  } else {
    sx1262_interface_delay_ms(tx_timeout);
    sx1262_interface_debug_print("Transmitting a packet works\n");

    sx1262_get_irq_status(&LoRa, &status);
    printf("IRQ Status: 0x%04X\n", status);
    // Check and print each flag
    if (status & SX1262_IRQ_TX_DONE) {
      printf("TX Done Interrupt\n");
    }
    if (status & SX1262_IRQ_RX_DONE) {
      printf("RX Done Interrupt\n");
    }
    if (status & SX1262_IRQ_PREAMBLE_DETECTED) {
      printf("Preamble Detected\n");
    }
    if (status & SX1262_IRQ_SYNC_WORD_VALID) {
      printf("Sync Word Valid\n");
    }
    if (status & SX1262_IRQ_HEADER_VALID) {
      printf("Header Valid\n");
    }
    if (status & SX1262_IRQ_HEADER_ERR) {
      printf("Header Error\n");
    }
    if (status & SX1262_IRQ_CRC_ERR) {
      printf("CRC Error\n");
    }
    if (status & SX1262_IRQ_CAD_DONE) {
      printf("CAD Done\n");
    }
    if (status & SX1262_IRQ_CAD_DETECTED) {
      printf("CAD Detected\n");
    }
    if (status & SX1262_IRQ_TIMEOUT) {
      printf("Timeout\n");
    }
    sx1262_clear_irq_status(&LoRa, 0x03FFU);
  }
}