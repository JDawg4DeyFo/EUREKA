/**
 * @file Sensors.c
 * @author Jacob Dennon (jdennon@ucsc.edu)
 * @brief Library for interfacing with sensors.
 * @version 0.1
 * @date 2025-03-08
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#include "../include/Sensors.h"

SensorsIDs_t (SenorsIDs_t Sensors) {
    SensorsIDs_t ReturnStatus = 0;

    // Check which sensors are to be initialized
    if(Sensors && SOIL) {
        // initialization code for soil sensor
        
        ReturnStatus |= SOIL; // indicate soil sensor was correctly initialized
    }

    if(Sensors && WIND) {
        // Initialization code

        ReturnStatus |= WIND;
    }


    if(Sensors && AIR) {
        // Initialization code

        ReturnStatus |= AIR;
    }

    if(Sensors && HUMID) {
        // Initialization code

        ReturnStatus |= HUMID;
    }

    return ReturnStatus;
}


SesnorErrors_t Soil_Read(short *Reading) {
    SesnorErrors_t ReturnStatus = 0;

    // I2C HAL CODE

    // data verification code

    return ReturnStatus;
}