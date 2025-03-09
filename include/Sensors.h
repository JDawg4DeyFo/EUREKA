/**
 * @file Sensors.h
 * @author Jacob Dennon (jdennon@ucsc.edu)
 * @brief Library for interfacing with sensors.
 * @version 0.1
 * @date 2025-03-08
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include "I2C.h"


/*******************************************************************************
 * PUBLIC #DEFINES                                                            *
 ******************************************************************************/
#define SOIL

/*******************************************************************************
 * PUBLIC DATATYPES
 ******************************************************************************/
// Associated bits for each sensor
typedef enum {
    SOIL = 0x1,
    WIND = 0x2,
    AIR = 0x4,
    HUMID = 0X8,
} SensorsIDs_t;

/*******************************************************************************
 * PUBLIC FUNCTIONS                                                           *
 ******************************************************************************/
/**
 * @brief This function intitialzes sensors according to a passed type def.
 * Return value is a bitewise description of sensors that have been initialized.
 * 
 * @param Sensors sensors to initialize
 * @return SensorsIDs_t Initialized sensors. 1 = initialized.
 */
SensorsIDs_t Sensors_Init(SensorsIDs_t Sensors);

short Soil_Read()

