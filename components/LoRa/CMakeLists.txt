## LoRa CMakeLists file
## April 16th, 2025

# Define source files
set(srcs Sensors.c SHT3X.c)

# Declare public dependencies
set(requires)

# Declare private dependencies
set(requires esp_driver_i2c)

# Register component
idf_component_register(SRCS "${srcs}"
    INCLUDE_DIRS "../../include"
	REQUIRES "${requires}")
