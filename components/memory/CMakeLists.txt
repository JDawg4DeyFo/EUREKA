## Memory CMakeLists file
## May 21st, 2025

# Define source files
set(srcs memory.c)

# Declare public dependencies
set(requires esp_driver_spi fatfs esp_driver_sdspi sdmmc)

# Declare private dependencies
set(priv_requires )

# Register component
idf_component_register(SRCS "${srcs}"
    INCLUDE_DIRS "../../include"
	REQUIRES "${requires}"
	PRIV_REQUIRES "${priv_requires}")
