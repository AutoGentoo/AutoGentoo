add_subdirectory(language)

add_library(autogentoo_cportage SHARED
        module.c expr.c expr.h)

setup_pymodule(autogentoo_cportage)

add_custom_target(autogentoo_cportage_test
        COMMAND ${LD_PRELOAD} ${CMAKE_CURRENT_SOURCE_DIR}/test.py
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/autogentoo_cportage.so
)

target_link_libraries(autogentoo_cportage PUBLIC cportage_language)
