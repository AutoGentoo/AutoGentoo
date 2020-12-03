add_library(autogentoo_network SHARED
        message.h
        tcp_server.c tcp_server.h module.c tcp_client.c tcp_client.h message.c ../cportage/portage.h ../python_util.h)

setup_pymodule(autogentoo_network)
target_link_libraries(autogentoo_network PUBLIC hacksaw)

add_custom_target(autogentoo_network_test
        COMMAND ${LD_PRELOAD} ${CMAKE_CURRENT_SOURCE_DIR}/test.py
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/autogentoo_network.so
)