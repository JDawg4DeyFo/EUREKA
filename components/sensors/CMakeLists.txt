## SENSORS CMakeLists file
## April 7th, 2025

# Define source files
set(srcs Sensors.c SH3X.c)

# Declare public dependencies
set(requires esp8266_wrapper)

# Declare private dependencies
set(requires esp_driver_i2c)

# Register component
idf_component_register(SRCS "${srcs}"
       	INCLUDE_DIRS "../../include"
	REQUIRES "${requires}")
