cmake_minimum_required(VERSION 3.10)
enable_language(C)
project(AutoGentoo VERSION "3.0.0")

SET(CMAKE_EXPORT_COMPILE_COMMANDS ON)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED YES)

# Setup the Python development environment
option(CUSTOM_PYTHON "Enable a custom path to a python library" OFF)
option(SANITIZER_BUILD "Build with AddressSanitizer" OFF)
set(ASAN_LIB "libasan.so" CACHE STRING "Path to address santizer library")

if (CUSTOM_PYTHON)
    if (Python3_USE_STATIC_LIBS)
        set(Python3_LIBRARY ${CUSTOM_PYTHON_ROOT}/lib/libpython${CUSTOM_PYTHON_VERSION}.a)
    else()
        set(Python3_LIBRARY ${CUSTOM_PYTHON_ROOT}/lib/libpython${CUSTOM_PYTHON_VERSION}.so)
    endif()
    set(Python3_EXECUTABLE ${CUSTOM_PYTHON_ROOT}/bin/python${CUSTOM_PYTHON_VERSION})
    set(Python3_INCLUDE_DIR ${CUSTOM_PYTHON_ROOT}/include/python${CUSTOM_PYTHON_VERSION})
    set(CMAKE_FIND_USE_CMAKE_PATH FALSE)
    set(Python3_ROOT_DIR ${CUSTOM_PYTHON_ROOT})
    message(STATUS "Building with Python${CUSTOM_PYTHON_VERSION} in ${CUSTOM_PYTHON_ROOT}")
endif()

set(Python3_FIND_VIRTUALENV "ONLY")
find_package(Python3 COMPONENTS Interpreter Development REQUIRED)

add_compile_definitions(_GNU_SOURCE)
add_compile_options(-Wno-unused-result -g -fno-omit-frame-pointer)
add_link_options(-fno-omit-frame-pointer)

set(EXEC_ENV "PYTHONPATH=${CMAKE_SOURCE_DIR}")
set(LD_PRELOAD "")
if (SANITIZER_BUILD)
    add_compile_options(-fsanitize=address)
    add_link_options(-fsanitize=address)

    set(LD_PRELOAD "LD_PRELOAD=${ASAN_LIB}")
    set(EXEC_ENV ${EXEC_ENV} LSAN_OPTIONS=suppressions=data/leak.supp)

    message(STATUS "Building with AddressSanitizer")
endif()

# This will compile all of the interface modules
include(cmake/pymodule.cmake)  # Python module macros

# Unit and integration testing
include(cmake/test.cmake)

add_subdirectory(third_party)
add_subdirectory(autogentoo)
