# Monolithic cmake file for EUREKA project
# April 3rd, 2025
cmake_minimum_required(VERSION 3.16)
include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(Eureka)


# Define build options
option(SENSOR_LIB_TEST "Test sensor library"s OFF)
# OPTIONS TO ADD:
	# main cluster head
	# main sensor node
	# main data sink
	# other library tests
	# experiment test harnesses

# Define source files conditionally
set(SRC_FILES "")

if (SENSOR_LIB_TEST)
	list(APPEND SRC_FILES "../src/I2C.c" "../src/Sesnors.c" "../SH3X.c")
	add_compile_definitions(SENSOR_LIB_TEST)
endif()

# Register component
idf_component_register(SRC ${SRC_FILES} INCLUDE_DIRS "../include")


#NOTE: To use build options, run idf.py as follows:
# idf.py -D SENSOR_LIB_TEST=ON build