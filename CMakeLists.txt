cmake_minimum_required(VERSION 3.15)
project(AutoGentoo VERSION "3.0.0")

SET( CMAKE_EXPORT_COMPILE_COMMANDS ON )

add_subdirectory(src)

if (SANITIZER_BUILD)
    set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
    set (CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
endif()
