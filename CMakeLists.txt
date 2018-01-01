cmake_minimum_required(VERSION 2.8.9)
project(AutoGentoo)
include_directories(include)

SET(CMAKE_C_COMPILER gcc)

file(GLOB SERVICE "service/*.c")
file(GLOB CLIENT "client/*.c")
file(GLOB HEADERS "include/*.h")

add_executable(autogentoo-daemon ${SERVICE} ${HEADERS})
#add_executable(autogentoo-client ${CLIENT} ${HEADERS})
TARGET_LINK_LIBRARIES(autogentoo-daemon curl archive hacksaw-tools)
