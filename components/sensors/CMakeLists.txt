## SENSORS CMakeLists file
## April 7th, 2025

# Define source files
set(srcs Sensors.c SH3X.c)

# Declare public dependencies
set(requires i2c esp8266_wrapper)

# No private dependencies to declare

# Register component
idf_component_register(SRCS "${srcs}"
       	INCLUDE_DIRS "../../include"
	REQUIRES "${requires}")
