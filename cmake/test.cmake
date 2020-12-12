function(setup_cmocka)
    include(${CMAKE_SOURCE_DIR}/third_party/cmocka/cmake/Modules/AddCMockaTest.cmake)
    include(${CMAKE_SOURCE_DIR}/cmake/AddMockedTest.cmake)
endfunction()

set(CMAKE_DISABLED_TESTS ${CMAKE_DISABLED_TESTS})

macro(set_tests_properties target)
    if (${target} IN_LIST CMAKE_DISABLED_TESTS)
    else()
        _set_tests_properties(${ARGV})
    endif()
endmacro()

setup_cmocka()
enable_testing()
add_subdirectory(${CMAKE_SOURCE_DIR}/autogentoo/test)