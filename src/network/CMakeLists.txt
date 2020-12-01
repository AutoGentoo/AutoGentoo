add_library(autogentoo_network SHARED
        message.h
        tcp_server.c tcp_server.h module.c tcp_client.c tcp_client.h message.c)

setup_pymodule(autogentoo_network)