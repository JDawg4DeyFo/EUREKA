## SENSORS CMakeLists file
## April 7th, 2025

# Define source files
set(SRC_FILES "Sensors.c" "SH3X.c")

# Declare public dependencies
set(REQUIRES i2c)

# No private dependencies to declare

# Register component
idf_component_register(SRC ${SRC_FILES}
       	INCLUDE_DIRS "../../include"
	REQUIRES)
