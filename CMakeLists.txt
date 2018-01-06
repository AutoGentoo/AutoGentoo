cmake_minimum_required(VERSION 3.10)
project(Hacksaw C)

FIND_PACKAGE(BISON REQUIRED)
FIND_PACKAGE(FLEX REQUIRED)

FLEX_TARGET(FLEX_ATOM ${CMAKE_SOURCE_DIR}/language/atom_flex.l
        ${CMAKE_CURRENT_BINARY_DIR}/atom.flex.c)
FLEX_TARGET(FLEX_DEPEND ${CMAKE_SOURCE_DIR}/language/depend_flex.l
        ${CMAKE_CURRENT_BINARY_DIR}/depend.flex.c)

BISON_TARGET(LANG_ATOM
        ${CMAKE_SOURCE_DIR}/language/atom_bison.y
        ${CMAKE_CURRENT_BINARY_DIR}/atom.tab.c)
BISON_TARGET(LANG_DEPEND
        ${CMAKE_SOURCE_DIR}/language/depend_bison.y
        ${CMAKE_CURRENT_BINARY_DIR}/depend.tab.c)

ADD_FLEX_BISON_DEPENDENCY(FLEX_ATOM LANG_ATOM)
ADD_FLEX_BISON_DEPENDENCY(FLEX_DEPEND LANG_DEPEND)

SET(LANG_BIN ${BISON_LANG_ATOM_OUTPUTS}
        ${BISON_LANG_DEPEND_OUTPUTS}
        ${FLEX_DEPEND_OUTPUTS}
        ${ATOM_DEPEND_OUTPUTS})

set(CMAKE_C_STANDARD 99)

include_directories (include)
include_directories (language)
include_directories (.)

file (GLOB HACKSAW_SRC "src/*.c")
file (GLOB HACKSAW_HEADERS "include/**/*.h")
file (GLOB HACKSAW_LANGUAGE_HEADERS "language/share.h")

add_library(hacksaw SHARED
        include/hacksaw.h
        include/tools.h
        ${HACKSAW_SRC}
        ${LANG_BINS}
        ${HACKSAW_LANGUAGE_HEADERS}
        ${HACKSAW_HEADERS}
        ${LANG_BIN})