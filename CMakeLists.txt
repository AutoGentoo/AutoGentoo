cmake_minimum_required(VERSION 2.8.9)
project(AutoGentoo)
include_directories(include)

SET(CMAKE_C_COMPILER gcc)

execute_process(
        COMMAND bash -c "mkdir -p hacksaw"
)

set(src_SOURCES
        src/autogentoo.c
        src/chroot.c
        src/command.c
        src/download.c
        src/handle.c
        src/host.c
        src/kernel.c
        src/response.c
        src/server.c
        src/stage.c
        src/writeconfig.c
        src/endian_convert.c
        src/getopt.c
        src/thread.c
        src/request.c
        src/request_structure.c
        src/http.c src/queue.c)

set(client_SOURCES
        src/render.c src/command_line.c)

set(client_HEADERS
        include/autogentoo/render.h include/autogentoo/command_line.h)

set(include_HEADERS
        include/autogentoo/autogentoo.h
        include/autogentoo/binpkg.h
        include/autogentoo/chroot.h
        include/autogentoo/command.h
        include/autogentoo/download.h
        include/autogentoo/handle.h
        include/autogentoo/host.h
        include/autogentoo/kernel.h
        include/autogentoo/response.h
        include/autogentoo/request.h
        include/autogentoo/server.h
        include/autogentoo/stage.h
        include/autogentoo/writeconfig.h
        include/autogentoo/endian_convert.h
        include/autogentoo/getopt.h
        include/autogentoo/thread.h
        include/autogentoo/request_structure.h
        include/autogentoo/http.h include/autogentoo/queue.h)

set(arch_SOURCES
        arch/main.c
        arch/aabs.c
        arch/db.c
        arch/package.c
        arch/repo.c
        arch/util.c
        arch/write.c
        arch/deps.c
        arch/ini.c
        arch/makepkg.c
        )

set(arch_HEADERS
        arch/aabs.h
        arch/db.h
        arch/package.h
        arch/repo.h
        arch/util.h
        arch/write.h
        arch/deps.h
        arch/ini.h
        arch/makepkg.h
        )

find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)

flex_target(FLEX_ATOM ${CMAKE_SOURCE_DIR}/hacksaw/language/atom_flex.l
        ${CMAKE_CURRENT_BINARY_DIR}/hacksaw/atom.flex.c)
flex_target(FLEX_DEPEND ${CMAKE_SOURCE_DIR}/hacksaw/language/depend_flex.l
        ${CMAKE_CURRENT_BINARY_DIR}/hacksaw/depend.flex.c)

bison_target(LANG_ATOM
        ${CMAKE_SOURCE_DIR}/hacksaw/language/atom_bison.y
        ${CMAKE_CURRENT_BINARY_DIR}/hacksaw/atom.tab.c)
bison_target(LANG_DEPEND
        ${CMAKE_SOURCE_DIR}/hacksaw/language/depend_bison.y
        ${CMAKE_CURRENT_BINARY_DIR}/hacksaw/depend.tab.c)

add_flex_bison_dependency(FLEX_ATOM LANG_ATOM)
add_flex_bison_dependency(FLEX_DEPEND LANG_DEPEND)

set(language_BIN
        ${BISON_LANG_ATOM_OUTPUTS}
        ${BISON_LANG_DEPEND_OUTPUTS}
        ${FLEX_FLEX_DEPEND_OUTPUTS}
        ${FLEX_FLEX_ATOM_OUTPUTS}
        )

include_directories(hacksaw/language)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(hacksaw_SRC
        hacksaw/src/conf.c
        hacksaw/src/debug.c
        hacksaw/src/dependency.c
        hacksaw/src/directory.c
        hacksaw/src/hash.c
        hacksaw/src/log.c
        hacksaw/src/manifest.c
        hacksaw/src/map.c
        hacksaw/src/package.c
        hacksaw/src/package_config.c
        hacksaw/src/regular_expression.c
        hacksaw/src/repository.c
        hacksaw/src/small_map.c
        hacksaw/src/string.c
        hacksaw/src/string_vector.c
        hacksaw/src/use_flag.c
        hacksaw/src/vector.c
        hacksaw/src/linked_vector.c
        hacksaw/src/util.c
        )

set(hacksaw_HEADERS
        include/autogentoo/hacksaw/hacksaw.h
        include/autogentoo/hacksaw/language.h
        include/autogentoo/hacksaw/tools.h
        include/autogentoo/hacksaw/portage/dependency.h
        include/autogentoo/hacksaw/portage/directory.h
        include/autogentoo/hacksaw/portage/hash.h
        include/autogentoo/hacksaw/portage/manifest.h
        include/autogentoo/hacksaw/portage/package.h
        include/autogentoo/hacksaw/portage/package_config.h
        include/autogentoo/hacksaw/portage/portage.h
        include/autogentoo/hacksaw/portage/repository.h
        include/autogentoo/hacksaw/portage/use_flags.h
        include/autogentoo/hacksaw/tools/conf.h
        include/autogentoo/hacksaw/tools/debug.h
        include/autogentoo/hacksaw/tools/log.h
        include/autogentoo/hacksaw/tools/map.h
        include/autogentoo/hacksaw/tools/regular_expression.h
        include/autogentoo/hacksaw/tools/small_map.h
        include/autogentoo/hacksaw/tools/string.h
        include/autogentoo/hacksaw/tools/string_vector.h
        include/autogentoo/hacksaw/tools/vector.h
        include/autogentoo/hacksaw/tools/linked_vector.h
        include/autogentoo/hacksaw/tools/util.h
        )

set(language_SOURCES
        hacksaw/language/share.h
        hacksaw/language/share.c
        hacksaw/language/atom.c
        hacksaw/language/atom.h
        hacksaw/language/depend.c
        hacksaw/language/depend.h
        )

set(language_PARSER
        hacksaw/language/atom_bison.y
        hacksaw/language/atom_flex.l
        hacksaw/language/depend_bison.y
        hacksaw/language/depend_flex.l
        )

add_library(hacksaw STATIC
        ${hacksaw_HEADERS}
        ${hacksaw_SRC}
        ${language_BIN}
        ${language_PARSER}
        ${language_SOURCES}
        )

add_executable(hacksaw-test hacksaw/src/main.c)
add_test(hacksaw-test hacksaw-test)
target_link_libraries(hacksaw-test hacksaw pcre2-8)

find_library(ARCHIVE archive)

set(LOCALSTATEDIR /var)
add_definitions(-DDBPATH="test")
#add_definitions(-DAUTOGENTOO_DEBUG)

add_executable(autogentoo-build ${arch_SOURCES} ${arch_HEADERS})
add_executable(autogentoo
        ${src_SOURCES}
        ${include_HEADERS}
        ${client_SOURCES}
        ${client_HEADERS})

TARGET_LINK_LIBRARIES(autogentoo curl
        ${ARCHIVE}
        pcre2-8
        hacksaw
        pthread
        curses
        )

TARGET_LINK_LIBRARIES(autogentoo-build curl ${ARCHIVE} pcre2-8 hacksaw)
