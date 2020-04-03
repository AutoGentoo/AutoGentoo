cmake_minimum_required(VERSION 3.8)
project(AutoGentoo VERSION "2.1.0")

SET(CMAKE_INSTALL_PREFIX /usr)

SET( CMAKE_EXPORT_COMPILE_COMMANDS ON )

find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)

include(cmake/autogentoo-build.txt)
include(cmake/autogentoo.txt)
include(cmake/hacksaw.txt)

include_directories(include)
include_directories(include/autogentoo)
include_directories(cportage/language)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

execute_process(COMMAND bash -c "mkdir -p {cportage,http}")

add_definitions(-DAUTOGENTOO_WORKER_DIR="${CMAKE_INSTALL_PREFIX}/share/autogentoo")
add_definitions(-DAUTOGENTOO_WORKER_DIR_DEBUG="${CMAKE_SOURCE_DIR}/src/worker")
add_definitions(-DDBPATH="test")
add_definitions(-DAUTOGENTOO_DEBUG)

add_library(autogentoo SHARED
        ${include_HEADERS}
        ${src_SOURCES}
        ${http_BIN}
        ${http_PARSER}
        ${worker_SOURCES}
        include/autogentoo/hacksaw/queue_set.h)

add_library(hacksaw SHARED
        ${hacksaw_HEADERS}
        ${hacksaw_SRC}
        hacksaw/queue_set.c)

find_package (Python COMPONENTS Interpreter Development)
add_library(autogentooapi SHARED
        include/autogentoo/api/host_serial.h
        src/api/host_serial.c
)

target_include_directories(autogentooapi PUBLIC ${Python_INCLUDE_DIRS})

target_link_libraries(autogentoo hacksaw)

#add_executable(autogentoo-build ${arch_SOURCES} ${arch_HEADERS})
add_executable(autogentoo-exec
        ${include_HEADERS}
        src/autogentoo.c
        )

add_subdirectory("src/api/")
set_target_properties(autogentoo-exec PROPERTIES OUTPUT_NAME autogentoo)
add_dependencies(autogentoo-exec autogentoo_api)

target_link_libraries(autogentoo-exec
        autogentoo
        pthread
        ssl
        crypto
        m
        )

add_executable(autogentoo-aabs
        ${arch_SOURCES}
        ${arch_HEADERS}
        )

install(TARGETS autogentoo DESTINATION lib)
install(TARGETS hacksaw DESTINATION lib)

add_subdirectory(cportage)

#add_executable(autogentoo-test
#        ${test_SOURCES}
#        )

#target_link_libraries(
#        autogentoo-test
#        cmocka
#        autogentoo
#        ssl
#        pthread
#        m
#)

target_link_libraries(
        autogentoo-aabs
        autogentoo
        archive
        ssl
        )

MACRO(INSTALL_HEADERS_WITH_DIRECTORY HEADER_LIST)
        FOREACH(HEADER ${${HEADER_LIST}})
                STRING(REGEX MATCH "(.*)[/]" DIR ${HEADER})
                INSTALL(FILES ${HEADER} DESTINATION ${DIR})
        ENDFOREACH(HEADER)
ENDMACRO(INSTALL_HEADERS_WITH_DIRECTORY)

SET(PYTHON_WORKER
        src/worker/cli.py
        src/worker/client.py
        src/worker/worker.py
        src/worker/script.py
        src/worker/util.py
        )
SET(WORKER_SCRIPTS
        src/worker/scripts/emerge.py
        src/worker/scripts/stage3.py
        src/worker/scripts/stage4.py
        src/worker/scripts/package_etc.py
        )


INSTALL(PROGRAMS ${PYTHON_WORKER} DESTINATION share/autogentoo/)
INSTALL(PROGRAMS ${WORKER_SCRIPTS} DESTINATION share/autogentoo/scripts/)

INSTALL_HEADERS_WITH_DIRECTORY(include_HEADERS)
INSTALL_HEADERS_WITH_DIRECTORY(hacksaw_HEADERS)

IF( EXISTS "${CMAKE_CURRENT_BINARY_DIR}/compile_commands.json" )
  EXECUTE_PROCESS( COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${CMAKE_CURRENT_BINARY_DIR}/compile_commands.json
    ${CMAKE_CURRENT_SOURCE_DIR}/compile_commands.json
  )
ENDIF()

