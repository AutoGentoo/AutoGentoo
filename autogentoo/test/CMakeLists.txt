macro(PythonUnitTest name)
    add_test(NAME ${name}_Python
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            COMMAND ${CMAKE_COMMAND} -E env
            ${LD_PRELOAD}
            ${EXEC_ENV}
            ${CMAKE_CURRENT_SOURCE_DIR}/${name}_test.py
            )
endmacro()

macro(CUnitTest name)
    add_mocked_test(${name}_C
            SOURCES ${name}_test.c
            LINK_LIBRARIES hacksaw Python3::Python autogentoo_${name}
            INCLUDE_DIRECTORIES ${Python_INCLUDE_DIRS}
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            ENVIRONMENT ${EXEC_ENV}
            )
endmacro()

CUnitTest(cportage)
PythonUnitTest(cportage)
PythonUnitTest(network)
