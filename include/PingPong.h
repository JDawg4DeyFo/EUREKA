/**
 * @file PingPong.h
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
#include "../../include/LoRa_main.h"

#define loop_delay_time 20
#define rx_delay_time 100

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

/**
 * @brief     Main function that takes care of the TX and RX states
 * @param[in] device_role either the device will begin transmitting (initiator) or receiving (responder)
 * @param[in] tx_msg payload to be sent
 * @note      none
 */
void PingPong(DeviceRole device_role, const char *tx_msg);

