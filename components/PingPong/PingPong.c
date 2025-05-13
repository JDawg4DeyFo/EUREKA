/**
 * @file PingPong.c
 * @author Edouard Valenzuela 
 * @brief Code for the function that will conduct the ping pong test
 * @version 0.1
 * @date 2025-04-30
 * 
 * @copyright Copyright (c) 2025
 * 
 * 
 *  
 */
#include "../../include/PingPong.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "esp_log.h"


static sx1262_handle_t LoRa;
static PingPongStates current_state;

/*If the device is a master, it will transmit a payload that is a string "Ping."
Else if the device is a slave, it will transmit a payload that is a string "Pong."*/

static const char *Debug_TAG = "ERROR";
static const char *RX_LENGTH_TAG = "Packet Length";
static const char *first_byte_TAG = "First Byte of the Last Packet Received";

uint8_t tx_buffer[15];
uint8_t rx_buffer[15];

void PingPong(DeviceRole device_role, const char *tx_msg){
    sx1262_lora_begin(&LoRa);

    if (!device_role){
        current_state = TRANSMIT;
    } else {
        current_state = RECEIVE;
    }

    while(1){
        uint32_t loop_delay = loop_delay_time;
        uint32_t receive_delay = rx_delay_time;

        uint16_t tx_buffer_len;
        uint8_t rx_buffer_len;

        switch(current_state){
            case TRANSMIT:
            {
                memcpy(tx_buffer, tx_msg, strlen(tx_msg) + 1);
                
                tx_buffer_len = sizeof(tx_buffer);
                sx1262_lora_set_send_mode(&LoRa);

                if(!sx1262_lora_send(&LoRa, tx_buffer, tx_buffer_len)){
                    current_state = RECEIVE;
                } else{
                    ESP_LOGI(Debug_TAG, "TX_DONE failed!\n");
                }
                break;
            }
            case RECEIVE: 
            {
                // Clear rx buffer
                memset(rx_buffer, 0, sizeof(rx_buffer));

                sx1262_lora_set_shot_receive_mode(&LoRa,receive_delay);
                sx1262_get_rx_buffer_status(&LoRa, &rx_buffer_len, rx_buffer);
                ESP_LOGI(RX_LENGTH_TAG, "Length of the packet received:\n");
                ESP_LOGI(first_byte_TAG, "First Byte of the buffer:\n");
                current_state = TRANSMIT;
                break;
            }
        }
        sx1262_interface_delay_ms(loop_delay);
    }
}
