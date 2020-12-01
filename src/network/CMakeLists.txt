add_library(autogentoo_network SHARED
        message.h
        tcp.c tcp.h)

setup_pymodule(autogentoo_network)