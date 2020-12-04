add_test(NAME cportage_unit
        COMMAND ${LD_PRELOAD} ${CMAKE_CURRENT_SOURCE_DIR}/cportage_unit.py
        )


add_mocked_test(cportage
        SOURCES cportage_test.c
        LINK_LIBRARIES hacksaw Python3::Python autogentoo_cportage
        INCLUDE_DIRECTORIES ${Python_INCLUDE_DIRS}
        )
