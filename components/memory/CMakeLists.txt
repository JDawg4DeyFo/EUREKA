## Memory CMakeLists file
## May 21st, 2025

# Define source files
set(srcs )

# Declare public dependencies
set(requires )

# Declare private dependencies
set(priv_requires )

# Register component
idf_component_register(SRCS "${srcs}"
    INCLUDE_DIRS "../../include"
	REQUIRES "${requires}"
	PRIV_REQUIRES "${priv_requires}")
