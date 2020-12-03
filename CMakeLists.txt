cmake_minimum_required(VERSION 3.15)
enable_language(C)
project(AutoGentoo VERSION "3.0.0")

SET(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(LD_PRELOAD "")

add_compile_definitions(_GNU_SOURCE)
add_compile_options(-Wno-unused-result -g -fno-omit-frame-pointer)
add_link_options(-fno-omit-frame-pointer)

if (SANITIZER_BUILD)
    add_compile_options(-fsanitize=address)
    add_link_options(-fsanitize=address)
    set(LD_PRELOAD "LD_PRELOAD=libasan.so")

    message("Building with AddressSanitizer")
endif()

add_subdirectory(src)
