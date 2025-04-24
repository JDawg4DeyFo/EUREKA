## Timer CMakeLists file
## April 7th, 2025

# Define source files
set(srcs Timer.c)

# Declare public dependencies
set(requires esp_driver_gptimer)

# Declare private dependencies
set(priv_requires)

# Register component
idf_component_register(SRCS "${srcs}"
       	INCLUDE_DIRS "../../include"
	REQUIRES "${requires}"
	PRIV_REQUIRES "${priv_requires}")