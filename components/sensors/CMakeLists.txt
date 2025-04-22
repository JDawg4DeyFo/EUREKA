## SENSORS CMakeLists file
## April 7th, 2025

# Define source files
set(srcs Sensors.c SHT3X.c)

# Declare public dependencies
set(requires timer esp_driver_pcnt)

# Declare private dependencies
set(priv_requires esp_driver_i2c esp_adc)

# Register component
idf_component_register(SRCS "${srcs}"
    INCLUDE_DIRS "../../include"
	REQUIRES "${requires}"
	PRIV_REQUIRES "${priv_requires}")
