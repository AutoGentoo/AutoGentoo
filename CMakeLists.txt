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
        src/worker/worker.h
        )

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


add_library(autogentoo SHARED
        ${include_HEADERS}
        ${src_SOURCES}
        ${http_BIN}
        ${http_PARSER}
        ${worker_SOURCES}
        )

target_link_libraries(autogentoo hacksaw)

#add_executable(autogentoo-build ${arch_SOURCES} ${arch_HEADERS})
add_executable(autogentoo-exec
        ${include_HEADERS}
        src/autogentoo.c
        )

add_executable(autogentoo-worker
        src/worker/worker_main.c
        )

target_link_libraries(autogentoo-worker
        autogentoo
        pthread
        ssl
        m
        )

target_link_libraries(autogentoo-exec
        autogentoo
        pthread
        ssl
        m
        )

add_executable(autogentoo-aabs
        ${arch_SOURCES}
        ${arch_HEADERS}
        )

add_executable(autogentoo-test
        ${test_SOURCES}
        )

target_link_libraries(
        autogentoo-test
        cmocka
        autogentoo
        ssl
        pthread
        m
)

target_link_libraries(
        autogentoo-aabs
        autogentoo
        ${ARCHIVE}
        ssl
        )

MACRO(INSTALL_HEADERS_WITH_DIRECTORY HEADER_LIST)
        FOREACH(HEADER ${${HEADER_LIST}})
                STRING(REGEX MATCH "(.*)[/]" DIR ${HEADER})
                INSTALL(FILES ${HEADER} DESTINATION ${DIR})
        ENDFOREACH(HEADER)

ENDMACRO(INSTALL_HEADERS_WITH_DIRECTORY)

SET(CMAKE_INSTALL_PREFIX /usr)

INSTALL(TARGETS autogentoo-exec DESTINATION bin)
INSTALL(TARGETS autogentoo hacksaw DESTINATION lib)

INSTALL_HEADERS_WITH_DIRECTORY(include_HEADERS)
INSTALL_HEADERS_WITH_DIRECTORY(hacksaw_HEADERS)