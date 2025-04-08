## I2C CMakeLists file
## April 8th, 2025

# Declare source files
set(srcs I2C.c)

# Declare include path
set(include_dirs ../../include)

# Declare piblic dependencies
set(requires)

# Declare private dependencies
set(priv_requires)

# NOTE: I think that the esspressif i2c drivers need to be listed as
# dependencies...

idf_component_register(SRCS "${srcs}"
	INCLUDE_DIRS "${include_dirs}"
	REQUIRES "${requires}"
	PRIV_REQUIRES "${priv_requires}")
