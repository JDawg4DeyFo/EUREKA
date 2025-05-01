/**
 * @file TX_Ping.c
 * @author Edouard Valenzuela 
 * @brief Code for the device that will start off transmitting in the PingPong test
 * @version 0.1
 * @date 2025-04-30
 * 
 * @copyright Copyright (c) 2025
 * 
 * 
 *  
 */
#include "../../include/LoRa_main.h"

#define loop_delay_time 20
#define tx_delay_time 1000

//List of states for the PingPong test
typedef enum{
    TRANSMIT = 0,
    RECEIVE = 1,
} PingPongStates;

//List of device roles for the PingPong test
typedef enum{
    INITIATOR = 0,
    RESPONDER = 1,
} DeviceRole;

static PingPongStates current_state;
static DeviceRole device_role;
static sx1262_handle_t LoRa;

/*If the device is a master, it will transmit a payload that is a string "Ping."
Else if the device is a slave, it will transmit a payload that is a string "Pong."*/
const char *ping_msg = "Ping";
const char *pong_msg = "Pong";

static const char *Debug_TAG = "ERROR";

uint8_t tx_buffer[15];
uint8_t rx_buffer[15];


void main(){
    device_role = INITIATOR;
    current_state = TRANSMIT;

    sx1262_lora_begin(&LoRa);

    while(1){
        uint32_t loop_delay = loop_delay_time;
        uint32_t transmit_delay = tx_delay_time;

        uint16_t tx_buffer_len;
        uint16_t rx_buffer_len;

        switch(current_state){
            case TRANSMIT:
            {
                if(!device_role){
                    memcpy(tx_buffer, ping_msg, strlen(ping_msg) + 1);
                }
                if(device_role){
                    memcpy(tx_buffer, pong_msg, strlen(pong_msg) + 1);
                }
                
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

                sx1262_lora_set_continuous_receive_mode(&LoRa);
                sx1262_interface_delay_ms(transmit_delay);


                current_state = TRANSMIT;
                break;
            }
        }
        sx1262_interface_delay_ms(loop_delay);
    }
}



