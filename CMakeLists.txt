cmake_minimum_required(VERSION 2.8.9)
project(AutoGentoo)
include_directories(include)

SET(CMAKE_C_COMPILER gcc)

execute_process(
        COMMAND bash -c "mkdir -p {hacksaw,http}"
)

set(src_SOURCES
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
        src/http.c
        src/queue.c
        src/gentoo_config.c
        src/user.c
        src/crypt.c
        src/pool.c
        src/server_loop.c
        hacksaw/src/server_handle.c
        hacksaw/src/server_init.c
        )

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
        include/autogentoo/http.h
        include/autogentoo/queue.h
        include/autogentoo/gentoo_config.h
        include/autogentoo/user.h
        include/autogentoo/crypt.h include/autogentoo/pool.h include/autogentoo/header.h)

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

# LANGUAGE
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

# HTTP
flex_target(FLEX_HTTP ${CMAKE_SOURCE_DIR}/src/http/ag_http_flex.l
        ${CMAKE_CURRENT_BINARY_DIR}/http/ag_http.flex.c)
bison_target(LANG_HTTP
        ${CMAKE_SOURCE_DIR}/src/http/ag_http_bison.y
        ${CMAKE_CURRENT_BINARY_DIR}/http/ag_http.tab.c)

add_flex_bison_dependency(FLEX_HTTP LANG_HTTP)

set(http_BIN
        ${BISON_LANG_HTTP_OUTPUTS}
        ${FLEX_FLEX_HTTP_OUTPUTS}
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
        hacksaw/src/server_handle.c)

set(http_PARSER
        src/http/ag_http_flex.l
        src/http/ag_http_bison.y
        )

add_library(hacksaw STATIC
        ${hacksaw_HEADERS}
        ${hacksaw_SRC}
        ${language_BIN}
        ${language_PARSER}
        ${language_SOURCES}
        )
set_target_properties(hacksaw
        PROPERTIES COMPILE_FLAGS "-fPIC")

find_library(ARCHIVE archive)

set(LOCALSTATEDIR /var)
add_definitions(-DDBPATH="test")
set(AUTH_ENCRYPT empty)
set(AUTH_ENCRYPT AUTH_LENGTH)

string(RANDOM LENGTH 2 ALPHABET 12346 AUTH_LENGTH)

string(RANDOM LENGTH ${AUTH_LENGTH} AUTH_ENCRYPT)
add_definitions(-DAUTOGENTOO_DEBUG)
#add_definitions(-DAUTOGENTOO_DEBUG)

add_library(autogentoo SHARED
        ${include_HEADERS}
        ${src_SOURCES}
        ${http_BIN}
        ${http_PARSER}
        )
target_link_libraries(autogentoo hacksaw)

#add_executable(autogentoo-build ${arch_SOURCES} ${arch_HEADERS})
add_executable(autogentoo-exec
        ${include_HEADERS}
        src/autogentoo.c

        )

target_link_libraries(autogentoo-exec
        curl
        ${ARCHIVE}
#        pcre2-8
        autogentoo
        pthread
        )

#target_link_libraries(autogentoo-build curl ${ARCHIVE}
##        pcre2-8
#        hacksaw)