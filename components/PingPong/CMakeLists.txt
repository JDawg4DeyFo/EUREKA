## Ping PongCMakeLists file
## May 1st, 2025

# Define source files
set(srcs PingPong.c)

# Declare public dependencies
set(requires LoRa)

# Declare private dependencies
set(priv_requires esp_driver_spi esp_driver_gpio)

# Register component
idf_component_register(SRCS "${srcs}"
    INCLUDE_DIRS "../../include"
	REQUIRES "${requires}"
	PRIV_REQUIRES "${priv_requires}")
