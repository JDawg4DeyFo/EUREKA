## I2C CMakeLists file
## April 8th, 2025

# Declare source files
set(srcs I2C.c)

# Declare include path
set(include_dirs ../../include)

# Declare piblic dependencies
set(requires driver)

# Declare private dependencies
set(priv_requires)

idf_component_register(SRCS "${srcs}"
	INCLUDE_DIRS "${include_dirs}"
	REQUIRES "${requires}"
	PRIV_REQUIRES "${priv_requires}")
