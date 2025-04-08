## Main CMakeLists.txt file
# This file handles conditional logic of component inclusion/exclusion
# April 7th, 2025

## BASIC DEFINES
set(srcs)
set(include_dirs "../include")
set(requires)
set(priv_requires)

## CONDITIONAL LOGIC###
## CONDITIONAL BUILDS TO BE IMPLEMENTED:
## (*) = might not be needed
#	Main Cluster program
#	Main sensor program
#	Lora test harness
#	SPI test harness (*)
#	ADC test harness (*)

# NOTE: Most of these conditional builds are conditionally exclusive, so I
# think having a elif tree for the conditional logic here makes sense.

# Sensor test harness build
if(SENSOR_LIB_TEST)
	append(srcs sensors_test.c)
	append(requires)
	append(priv_requires sensors i2c)
endif()

# Register main as the component
idf_component_register(SRCS "${src}"
                       INCLUDE_DIRS "${include_dirs}"
                       REQUIRES "${requires}"
		       PRIV_REQUIRES "${priv_requires}")
