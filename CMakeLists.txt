cmake_minimum_required(VERSION 3.15)
enable_language(C)
project(AutoGentoo VERSION "3.0.0")

SET(CMAKE_EXPORT_COMPILE_COMMANDS ON)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED YES)

set(LD_PRELOAD "")

# Setup the Python development environment
set(Python3_FIND_VIRTUALENV "ONLY")
find_package(Python3 COMPONENTS Development)

add_compile_definitions(_GNU_SOURCE)
add_compile_options(-Wno-unused-result -g -fno-omit-frame-pointer)
add_link_options(-fno-omit-frame-pointer)

if (SANITIZER_BUILD)
    add_compile_options(-fsanitize=address)
    add_link_options(-fsanitize=address)
    set(LD_PRELOAD "LD_PRELOAD=libasan.so")

    message("-- Building with AddressSanitizer")
endif()

# This will compile all of the interface modules
include(cmake/pymodule.cmake)  # Python module macros

# Unit and integration testing
include(cmake/test.cmake)

add_subdirectory(src)
add_subdirectory(third_party)
