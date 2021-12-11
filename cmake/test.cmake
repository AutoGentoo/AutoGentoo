function(setup_cmocka)
    include(${CMAKE_SOURCE_DIR}/third_party/cmocka/cmake/Modules/AddCMockaTest.cmake)
    include(${CMAKE_SOURCE_DIR}/cmake/AddMockedTest.cmake)
endfunction()

setup_cmocka()
add_subdirectory(${CMAKE_SOURCE_DIR}/autogentoo/test)