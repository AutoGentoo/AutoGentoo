cmake_minimum_required(VERSION 2.8)
project(AutoGentoo)

SET(CMAKE_INSTALL_PREFIX /usr)

find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)

include(cmake/autogentoo-build.txt)
include(cmake/autogentoo.txt)
include(cmake/hacksaw.txt)
include (${CMAKE_ROOT}/Modules/FindPythonLibs.cmake)
include (${CMAKE_ROOT}/Modules/FindPythonInterp.cmake)

find_package(PythonLibs)

include_directories(include)
include_directories(cportage/language)
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${PYTHON_INCLUDE_DIR})

#find_package (Python3 COMPONENTS Interpreter Development)

execute_process(COMMAND bash -c "mkdir -p {cportage,http}")

#add_definitions(-DAUTOGENTOO_WORKER_DIR="${CMAKE_INSTALL_PREFIX}/share/autogentoo")
add_definitions(-DAUTOGENTOO_WORKER_DIR="${CMAKE_SOURCE_DIR}/src/worker")
add_definitions(-DDBPATH="test")
add_definitions(-DAUTOGENTOO_DEBUG)

add_library(autogentoo SHARED
        ${include_HEADERS}
        ${src_SOURCES}
        ${http_BIN}
        ${http_PARSER}
        ${worker_SOURCES}
        )

add_library(cportage SHARED
        ${src_CPORTAGE}
        ${language_BIN}
        ${language_PARSER}
        ${language_SOURCES}
        )

add_library(hacksaw SHARED
        ${hacksaw_HEADERS}
        ${hacksaw_SRC}
        )

target_link_libraries(autogentoo hacksaw)
target_link_libraries(cportage hacksaw)

#add_executable(autogentoo-build ${arch_SOURCES} ${arch_HEADERS})
add_executable(autogentoo-exec
        ${include_HEADERS}
        src/autogentoo.c
        )

set_target_properties(autogentoo-exec PROPERTIES OUTPUT_NAME autogentoo)

add_executable(cportage-exec
        cportage/cemerge.c
        )

set_target_properties(cportage-exec PROPERTIES OUTPUT_NAME autogentoo-emerge)

target_link_libraries(cportage-exec
        cportage
        ssl
        crypto
        archive
        )


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

INSTALL(TARGETS autogentoo-exec cportage-exec DESTINATION bin)
INSTALL(TARGETS autogentoo cportage hacksaw DESTINATION lib)

SET(PYTHON_WORKER
        src/worker/cli.py
        src/worker/client.py
        src/worker/worker.py
        src/worker/script.py
        )
SET(WORKER_SCRIPTS
        src/worker/scripts/stage3.py
        )


INSTALL(PROGRAMS ${PYTHON_WORKER} DESTINATION share/autogentoo/)
INSTALL(PROGRAMS ${WORKER_SCRIPTS} DESTINATION share/autogentoo/scripts/)

INSTALL_HEADERS_WITH_DIRECTORY(include_HEADERS)
INSTALL_HEADERS_WITH_DIRECTORY(hacksaw_HEADERS)
