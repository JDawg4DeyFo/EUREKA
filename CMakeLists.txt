# Monolithic cmake file for EUREKA project
# April 3rd, 2025
cmake_minimum_required(VERSION 3.16)
include($ENV{IDF_PATH}/tools/cmake/project.cmake)
set(EXTRA_COMPONENT_DIRS ${CMAKE_SOURCE_DIR}/esp-lib/components)

project(Eureka)
