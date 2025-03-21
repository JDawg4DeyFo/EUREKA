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
#define SOIL_ADDRESS 0x38;      // I2C Bus address
// NEED TO FIND VALUES FOR DEFINES BELOW
#define SOIL_REGISTER 0x00;     // find what sensor to read to
#define WIND_ADDRESS 0x00;
#define AIR_ADDRESS 0X00;
#define HUMID_ADDRESS 0x00;

#define STEMMA_SENSOR_ADDR 0x36
#define STEMMA_MOISTURE_BASE_REG 0x0F
#define STEMMA_MOISTURE_FUNC_REG 0x10
#define STEMMA_TEMP_BASE_REG 0x00
#define STEMMA_TEMP_FUNC_REG 0x04

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

// Possible errors.
// 1st draft, will need to add or remove as needed
typedef enum {
    NO_ERROR = 0,
    ERROR = 0x1,
    NO_RESPONSE = 0x2,
    BUS_COLISION = 0x4,
} SesnorErrors_t;

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


/**
 * @brief Read from the soil moisture sensor
 * 
 * @param Reading short to store reading into
 * @return ESP error type
 */
esp_err_t Read_SoilMoisture(short *Reading);

/**
 * @brief Read soil temperature from addafruit sensor
 * 
 * @param Reading float to store reading into
 * @return ESP error type
 */
esp_err_t Read_SoilTemperature(float *Reading);

/**
 * @brief Read air temperature for sht30 humidity and temperature sensor.
 * 
 * @return float 
 */
esp_err_t Read_AirTemperature(float *Reading);

/**
 * @brief Read air humidity for sht30 humidity and temperature sensor.
 * 
 * @param Reading float to store reading into
 * @return float 
 */
esp_err_t Read_AirHumidity(float *Reading);