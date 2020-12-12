function(add_mocked_test name)
    # parse arguments passed to the function
    set(options )
    set(oneValueArgs )
    set(multiValueArgs
            SOURCES
            COMPILE_OPTIONS
            LINK_LIBRARIES
            LINK_OPTIONS
            INCLUDE_DIRECTORIES
            WORKING_DIRECTORY
            ENVIRONMENT
            )
    cmake_parse_arguments(ADD_MOCKED_TEST "${options}" "${oneValueArgs}"
      "${multiValueArgs}" ${ARGN} )

    # define test
    add_cmocka_test(test_${name}
                    SOURCES ${ADD_MOCKED_TEST_SOURCES}
                    COMPILE_OPTIONS ${ADD_MOCKED_TEST_COMPILE_OPTIONS}
                    LINK_LIBRARIES cmocka
                                   ${ADD_MOCKED_TEST_LINK_LIBRARIES}
                    LINK_OPTIONS ${ADD_MOCKED_TEST_LINK_OPTIONS}
                    WORKING_DIRECTORY ${ADD_MOCKED_TEST_WORKING_DIRECTORY}
                    ENVIRONMENT ${ADD_MOCKED_TEST_ENVIRONMENT}
            )
 
    # allow using includes from autogentoo/ directory
    target_include_directories(test_${name} PRIVATE ${CMAKE_SOURCE_DIR}/src ${INCLUDE_DIRECTORIES})
endfunction(add_mocked_test)