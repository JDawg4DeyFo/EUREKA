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
#include <math.h>

#include "driver/pulse_cnt.h"

/*******************************************************************************
 * PUBLIC #DEFINES                                                            *
 ******************************************************************************/
// Soil Sensor I2C Addresses
#define STEMMA_SENSOR_ADDR 0x36
#define STEMMA_MOISTURE_BASE_REG 0x0F
#define STEMMA_MOISTURE_FUNC_REG 0x10
#define STEMMA_TEMP_BASE_REG 0x00
#define STEMMA_TEMP_FUNC_REG 0x04
#define STEMMA_STATUS_BASE_REG 0x00
#define STEMMA_STATUS_HWID_REG 0x01

#define SOIL_MOISTURE_DATA_LENGTH 2
#define SOIL_TEMP_DATA_LENGTH 4

// sht3x defines
#define SHT3X_REPEATABILITY sht3x_low // both are defined in SH3X.h
#define SHT3X_PERIOD sht3x_single_shot


// Weathervane defines
#define NUMBER_OF_KEYS 16
#define KEY_TO_DEG 22.5
#define ANEMOMETER_GPIO	CONFIG_ANEMOMETER_GPIO
#define WINDVANE_GPIO	CONFIG_WINDVANE_GPIO
// #define ANEMOMETER_VELOCITY_CONSTANT 0.6666 // m/s
#define ANEMOMETER_VELOCITY_CONSTANT 2.4 // km/h

//	 ADC defines
#define ADC_BITWIDTH 12.0
#define MAX_ADC_VOLTAGE 3.3

// PCNT defines
#define PCNT_HIGH_LIMIT 10
#define PCNT_LOW_LIMIT -10

/*******************************************************************************
 * PUBLIC DATATYPES
 ******************************************************************************/
// Associated bits for each sensor
typedef enum {
    SOIL = 0x1,
    WINDVANE = 0x2,
	ANEMOMETER = 0x4,
    SHT30 = 0X8,
} SensorsIDs_t;
#define ALL_SENSORS 0xF

// Possible errors.
// 1st draft, will need to add or remove as needed
typedef enum {
    NO_ERROR = 0,
    ERROR = 0x1,
    NO_RESPONSE = 0x2,
    BUS_COLISION = 0x4,
} SesnorErrors_t;

typedef struct {
	uint64_t Duration;
	uint64_t StartTime;
	uint64_t EndTime;
	pcnt_unit_handle_t *PCNTHandle;
	int TotalIterations;
} PCNT_State_t;

/*******************************************************************************
 * PUBLIC FUNCTIONS                                                           *
 ******************************************************************************/
/**
 * @brief This function intitialzes sensors according to a passed type def.
 * Return value is a bitewise description of sensors that have been initialized.
 * Most sensors don't require specific intitialization code past proper
 * configuration of the I2C Bus. I2C_Init() should be called beforehand.
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
 * @brief Read air humidity for sht30 humidity and temperature sensor.
 * 
 * @param Temp_Reading float to store temperature reading into
 * @param Humid_Reading float to store humidity reading into
 * @return bool, true success, false for fail 
 */
bool Read_SHT30_HumidityTemperature(float *Temp_Reading, float *Humid_Reading);

/**
 * @brief Get wind direction from ADC reading
 * 
 * @return short bearing, in degrees 
 */
float Get_Wind_Direction(void);

/**
 * @brief Get wind speed from anemometer pulse width
 * 
 * @return float wind speed, in meters per second
 */
float Get_Wind_Speed(void);

/**
 * @brief Deinitialize sensors and their associated modules.
 * 
 * @return true 
 * @return false 
 */
bool Deinitialize_Sensors(void);
