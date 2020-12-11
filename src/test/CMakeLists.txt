add_test(NAME cportage_unit
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMAND ${CMAKE_COMMAND} -E env
            PYTHONPATH=${CMAKE_BINARY_DIR}/src/cportage/  # Used to help import the module with out 'make install'
            ${CMAKE_CURRENT_SOURCE_DIR}/cportage_unit.py
        )


add_mocked_test(cportage
        SOURCES cportage_test.c
        LINK_LIBRARIES hacksaw Python3::Python autogentoo_cportage
        INCLUDE_DIRECTORIES ${Python_INCLUDE_DIRS}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        )
