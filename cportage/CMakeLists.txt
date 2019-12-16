cmake_minimum_required(VERSION 3.8)
project(cportage VERSION "1.2.0" LANGUAGES C)

SET(CMAKE_INSTALL_PREFIX /usr)
SET(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)

include(files.txt)
include_directories(src/language)

execute_process(COMMAND bash -c "mkdir -p ${PROJECT_BINARY_DIR}/lang")

add_library(cportage SHARED
        ${include_CPORTAGE}
        ${src_CPORTAGE}
        ${language_BIN}
        ${language_PARSER}
        ${language_SOURCES}
        src/dependency.c src/dependency.h src/resolve.c src/resolve.h src/selected.c src/selected.h src/block.c src/block.h)

target_link_libraries(cportage hacksaw)

add_executable(cportage-exec
        src/cemerge.c
        )

target_link_libraries(cportage-exec
        cportage
        ssl
        crypto
        archive
        )

install(DIRECTORY src/ DESTINATION include/cportage
        FILES_MATCHING PATTERN "*.h")

INSTALL(TARGETS cportage-exec DESTINATION bin)
INSTALL(TARGETS cportage DESTINATION lib)

IF( EXISTS "${CMAKE_CURRENT_BINARY_DIR}/compile_commands.json" )
    EXECUTE_PROCESS( COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_CURRENT_BINARY_DIR}/compile_commands.json
            ${CMAKE_CURRENT_SOURCE_DIR}/compile_commands.json
            )
ENDIF()