add_library(autogentoo_network SHARED
        message.c
        message.h
        )

setup_pymodule(autogentoo_network)