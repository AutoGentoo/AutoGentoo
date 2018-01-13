cmake_minimum_required (VERSION 3.0)
project (Hacksaw C)

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

set (language_BIN
        ${BISON_LANG_ATOM_OUTPUTS}
        ${BISON_LANG_DEPEND_OUTPUTS}
        ${FLEX_FLEX_DEPEND_OUTPUTS}
        ${FLEX_FLEX_ATOM_OUTPUTS}
        )

set (CMAKE_C_STANDARD 99)

include_directories (include)
include_directories (language)
include_directories (.)
include_directories (${CMAKE_CURRENT_BINARY_DIR})

set (hacksaw_SRC
        src/conf.c
        src/debug.c
        src/dependency.c
        src/directory.c
        src/hash.c
        src/log.c
        src/manifest.c
        src/map.c
        src/package.c
        src/package_config.c
        src/regular_expression.c
        src/repository.c
        src/small_map.c
        src/string.c
        src/string_vector.c
        src/use_flag.c
        src/vector.c
        src/linked_vector.c
        src/util.c
        )

set (hacksaw_HEADERS
        include/hacksaw.h
        include/tools.h
        include/portage/dependency.h
        include/portage/directory.h
        include/portage/hash.h
        include/portage/manifest.h
        include/portage/package.h
        include/portage/package_config.h
        include/portage/portage.h
        include/portage/repository.h
        include/portage/use_flags.h
        include/tools/conf.h
        include/tools/debug.h
        include/tools/log.h
        include/tools/map.h
        include/tools/regular_expression.h
        include/tools/small_map.h
        include/tools/string.h
        include/tools/string_vector.h
        include/tools/vector.h
        include/tools/linked_vector.h
        include/tools/util.h
        )

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

add_library (hacksaw SHARED
        ${hacksaw_HEADERS}
        ${hacksaw_SRC}
        ${language_BIN}
        ${language_PARSER}
        ${language_SOURCES}
        )

add_executable (hacksaw-test EXCLUDE_FROM_ALL src/main.c)
add_test (hacksaw-test hacksaw-test)
target_link_libraries (hacksaw-test hacksaw pcre2-8)
