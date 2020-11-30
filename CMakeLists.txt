cmake_minimum_required(VERSION 3.8)
project(AutoGentoo VERSION "2.1.0")

SET( CMAKE_EXPORT_COMPILE_COMMANDS ON )

find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)

include(cmake/hacksaw.cmake)

include_directories(include)

add_library(hacksaw STATIC
        ${hacksaw_HEADERS}
        ${hacksaw_SRC})
