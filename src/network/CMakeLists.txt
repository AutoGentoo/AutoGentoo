PythonModule(autogentoo_network
        SOURCES
        message.h  tcp_server.c tcp_server.h module.c
        tcp_client.c tcp_client.h message.c ../python_util.h
        LINK_LIBRARIES hacksaw
        )

add_custom_target(autogentoo_network_test
        COMMAND ${LD_PRELOAD} ${CMAKE_CURRENT_SOURCE_DIR}/test.py
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/autogentoo_network.so
)