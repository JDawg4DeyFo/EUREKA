/**
 * @file Timer.h
 * @author Jacob Dennon (jdennon@ucsc.edu)
 * @brief Header file for general purpose timer (configured as a FRT)
 * @version 0.1
 * @date 2025-04-17
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gptimer.h"
#include "esp_log.h"


/**
 * @brief Initialize the free running timer
 * 
 */
void FreeRunningTimer_Init(void);

/**
 * @brief De-initialize the free running timer
 * 
 */
void FreeRunningTimer_Deinit(void);