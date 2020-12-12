PythonModule(autogentoo_network
        SOURCES
        message.h  tcp_server.c tcp_server.h module.c
        tcp_client.c tcp_client.h message.c ../python_util.h
        LINK_LIBRARIES hacksaw
        )
