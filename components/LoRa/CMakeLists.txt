set(component_srcs "LoRa.c")

idf_component_register(SRCS "${component_srcs}"
                       PRIV_REQUIRES driver
                       INCLUDE_DIRS ".")