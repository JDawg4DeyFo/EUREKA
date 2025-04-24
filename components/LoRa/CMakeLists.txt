## LoRa CMakeLists file
## April 7th, 2025

# Define source files
set(srcs LoRa_driver.c LoRa_interface.c)

# Declare public dependencies
set(requires esp_driver_spi)

# Declare private dependencies
set(priv_requires)

# Register component
idf_component_register(SRCS "${srcs}"
    INCLUDE_DIRS "../../include"
	REQUIRES "${requires}"
	PRIV_REQUIRES "${priv_requires}")
