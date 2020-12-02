cmake_minimum_required(VERSION 3.15)
project(AutoGentoo VERSION "3.0.0")

SET(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_C_CFLAGS "${CMAKE_C_FLAGS} -Wno-unused-result")
set(LD_PRELOAD "")

if (SANITIZER_BUILD)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fno-omit-frame-pointer -fsanitize=address")
    set (CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} -fno-omit-frame-pointer  -fsanitize=address -shared-libasan")
    set(LD_PRELOAD "LD_PRELOAD=libasan.so")

    message("Building with AddressSanitizer")
endif()

add_subdirectory(src)
