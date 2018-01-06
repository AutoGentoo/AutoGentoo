cmake_minimum_required(VERSION 3.10)
project(Hacksaw C)

find_package (BISON REQUIRED)
find_package(FLEX REQUIRED)

flex_target (FLEX_ATOM ${CMAKE_SOURCE_DIR}/language/atom_flex.l
        ${CMAKE_CURRENT_BINARY_DIR}/atom.flex.c)
flex_target (FLEX_DEPEND ${CMAKE_SOURCE_DIR}/language/depend_flex.l
        ${CMAKE_CURRENT_BINARY_DIR}/depend.flex.c)

bison_target (LANG_ATOM
        ${CMAKE_SOURCE_DIR}/language/atom_bison.y
        ${CMAKE_CURRENT_BINARY_DIR}/atom.tab.c)
bison_target (LANG_DEPEND
        ${CMAKE_SOURCE_DIR}/language/depend_bison.y
        ${CMAKE_CURRENT_BINARY_DIR}/depend.tab.c)

add_flex_bison_dependency (FLEX_ATOM LANG_ATOM)
add_flex_bison_dependency (FLEX_DEPEND LANG_DEPEND)

set (LANG_BIN ${BISON_LANG_ATOM_OUTPUTS}
        ${BISON_LANG_DEPEND_OUTPUTS}
        ${FLEX_DEPEND_OUTPUTS}
        ${ATOM_DEPEND_OUTPUTS})

set (CMAKE_C_STANDARD 99)

include_directories (include)
include_directories (language)
include_directories (.)

file (GLOB HACKSAW_SRC "src/*.c")
file (GLOB HACKSAW_HEADERS "include/**/*.h")

set (language_SOURCES
        language/share.h
        language/share.c
        language/atom.c
        language/atom.h
        language/depend.c
        language/depend.h
        )

set (language_PARSER
        language/atom_bison.y
        language/atom_flex.l
        language/depend_bison.y
        language/depend_flex.l
        )

add_library(hacksaw SHARED
        include/hacksaw.h
        include/tools.h
        ${HACKSAW_SRC}
        ${LANG_BINS}
        ${language_SOURCES}
        ${HACKSAW_HEADERS}
        ${LANG_BIN}
        ${language_PARSER}
        )
