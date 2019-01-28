cmake_minimum_required(VERSION 2.8.9)
project(AutoGentoo)

include(cmake/hacksaw.txt)
include(cmake/autogentoo-build.txt)
include(cmake/autogentoo.txt)
include (${CMAKE_ROOT}/Modules/FindPythonLibs.cmake)
include (${CMAKE_ROOT}/Modules/FindPythonInterp.cmake)

find_package(PythonLibs)

include_directories(include)
include_directories(hacksaw/language)
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${PYTHON_INCLUDE_DIR})

#find_package (Python3 COMPONENTS Interpreter Development)

SET(CMAKE_C_COMPILER gcc)

execute_process(COMMAND bash -c "mkdir -p {hacksaw,http}")

set(worker_SOURCES
        src/worker/worker.c
        src/worker/worker.h)

add_library(hacksaw STATIC
        ${hacksaw_HEADERS}
        ${hacksaw_SRC}
        ${language_BIN}
        ${language_PARSER}
        ${language_SOURCES}
        )

set_target_properties(hacksaw PROPERTIES COMPILE_FLAGS "-fPIC")

find_library(ARCHIVE archive)

set(LOCALSTATEDIR /var)
add_definitions(-DDBPATH="test")
add_definitions(-DAUTOGENTOO_DEBUG)
#add_definitions(-DAUTOGENTOO_DEBUG)

add_library(worker STATIC
        ${worker_SOURCES}
        )

add_library(autogentoo SHARED
        ${include_HEADERS}
        ${src_SOURCES}
        ${http_BIN}
        ${http_PARSER}
        )
target_link_libraries(autogentoo hacksaw worker)

#add_executable(autogentoo-build ${arch_SOURCES} ${arch_HEADERS})
add_executable(autogentoo-exec
        ${include_HEADERS}
        src/autogentoo.c
        )

target_link_libraries(autogentoo-exec
        ${ARCHIVE}
        autogentoo
        pthread
        ${PYTHON_LIBRARY}
        )

add_executable(autogentoo-aabs
        ${arch_SOURCES}
        ${arch_HEADERS}
        )

target_link_libraries(
        autogentoo-aabs
        curl
        autogentoo
        ${ARCHIVE}
        ssl
        )