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

message(STATUS "Value of SENSOR_LIB_TEST: ${CONFIG_SENSOR_LIB_TEST}")

# Sensor test harness build
if(CONFIG_SENSOR_LIB_TEST)
	list(APPEND srcs SensorLibTest.c)
	list(APPEND requires)
	list(APPEND priv_requires sensors)
elseif(CONFIG_LORA_LIB_TEST)
	list(APPEND srcs LoraLibTest.c)
	list(APPEND requires)
	list(APPEND priv_requires LoRa)
elseif(CONFIG_TX_PINGPONG_TEST)
	list(APPEND srcs TX_Ping.c)
	list(APPEND requires)
	list(APPEND priv_requires PingPong)
elseif(CONFIG_RX_PINGPONG_TEST)
	list(APPEND srcs RX_Pong.c)
	list(APPEND requires)
	list(APPEND priv_requires PingPong)
endif()

# Register main as the component
idf_component_register(SRCS "${srcs}"
                       INCLUDE_DIRS "${include_dirs}"
                       REQUIRES "${requires}"
		       PRIV_REQUIRES "${priv_requires}")
