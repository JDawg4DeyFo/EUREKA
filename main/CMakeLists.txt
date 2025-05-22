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
elseif(CONFIG_JACOB_LORA)
	list(APPEND srcs JacobLoraDebug.c)
	list(APPEND requires)
	list(APPEND priv_requires LoRa)
elseif(CONFIG_PCNT_TEST)
	list(APPEND srcs )
	list(APPEND requires)
	list(APPEND priv_requires)
elseif(CONFIG_POWER_TEST)
	list(APPEND srcs PowerTest.c)
	list(APPEND requires)
	list(APPEND priv_requires sensors esp_timer LoRa)
elseif(CONFIG_SENSOR_NODE_MAIN)
	list(APPEND srcs SensorMain.c)
	list(APPEND requires)
	list(APPEND priv_requires sensors esp_timer LoRa)
elseif(CONFIG_CLUSTER_HEAD_MAIN)
	list(APPEND srcs ClusterMain.c)
	list(APPEND requires)
	list(APPEND priv_requires esp_timer LoRa)
elseif(CONFIG_NEW_DRIVER_TEST)
	list(APPEND srcs NewDriverTest.c)
	list(APPEND requires)
	list(APPEND priv_requires LoRa)
elseif(CONFIG_PINGPONG_TEST)
	list(APPEND srcs PingPongTest.c)
	list(APPEND requires)
	list(APPEND priv_requires LoRa)
elseif(CONFIG_FOREST_PATHLOSS_TEST)
	list(APPEND srcs ForestPathLossTest.c)
	list(APPEND requires)
	list(APPEND priv_requires LoRa)
elseif(CONFIG_LORA_CAD_TEST)
	list(APPEND srcs LoRaCADTest.c)
	list(APPEND requires)
	list(APPEND priv_requires LoRa)
elseif(CONFIG_MEMORY_LIB_TEST)
	list(APPEND srcs memoryLibTest.c)
	list(APPEND requires)
	list(APPEND priv_requires memory)
endif()

# Register main as the component
idf_component_register(SRCS "${srcs}"
                    INCLUDE_DIRS "${include_dirs}"
            		REQUIRES "${requires}"
		       		PRIV_REQUIRES "${priv_requires}")
