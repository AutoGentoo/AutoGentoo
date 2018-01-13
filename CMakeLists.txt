cmake_minimum_required(VERSION 2.8.9)
project(AutoGentoo)
include_directories(include)

SET(CMAKE_C_COMPILER gcc)

execute_process (
        COMMAND bash -c "mkdir -p hacksaw"
)


set (service_SOURCES
        service/autogentoo.c
        service/chroot.c
        service/command.c
        service/download.c
        service/handle.c
        service/host.c
        service/kernel.c
        service/response.c
        service/server.c
        service/stage.c
        service/writeconfig.c
        service/endian_convert.c
        )

set (include_HEADERS
        include/autogentoo.h
        include/binpkg.h
        include/chroot.h
        include/command.h
        include/download.h
        include/handle.h
        include/host.h
        include/kernel.h
        include/response.h
        include/server.h
        include/stage.h
        include/writeconfig.h
        include/endian_convert.h
        )

set (arch_SOURCES
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

set (arch_HEADERS
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

find_package (BISON REQUIRED)
find_package(FLEX REQUIRED)

flex_target (FLEX_ATOM ${CMAKE_SOURCE_DIR}/hacksaw/language/atom_flex.l
        ${CMAKE_CURRENT_BINARY_DIR}/hacksaw/atom.flex.c)
flex_target (FLEX_DEPEND ${CMAKE_SOURCE_DIR}/hacksaw/language/depend_flex.l
        ${CMAKE_CURRENT_BINARY_DIR}/hacksaw/depend.flex.c)

bison_target (LANG_ATOM
        ${CMAKE_SOURCE_DIR}/hacksaw/language/atom_bison.y
        ${CMAKE_CURRENT_BINARY_DIR}/hacksaw/atom.tab.c)
bison_target (LANG_DEPEND
        ${CMAKE_SOURCE_DIR}/hacksaw/language/depend_bison.y
        ${CMAKE_CURRENT_BINARY_DIR}/hacksaw/depend.tab.c)

add_flex_bison_dependency (FLEX_ATOM LANG_ATOM)
add_flex_bison_dependency (FLEX_DEPEND LANG_DEPEND)

set (language_BIN
        ${BISON_LANG_ATOM_OUTPUTS}
        ${BISON_LANG_DEPEND_OUTPUTS}
        ${FLEX_FLEX_DEPEND_OUTPUTS}
        ${FLEX_FLEX_ATOM_OUTPUTS}
        )


include_directories (hacksaw/include)
include_directories (hacksaw/language)
include_directories (hacksaw/)
include_directories (${CMAKE_CURRENT_BINARY_DIR})

set (hacksaw_SRC
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

set (hacksaw_HEADERS
        hacksaw/include/hacksaw.h
        hacksaw/include/tools.h
        hacksaw/include/portage/dependency.h
        hacksaw/include/portage/directory.h
        hacksaw/include/portage/hash.h
        hacksaw/include/portage/manifest.h
        hacksaw/include/portage/package.h
        hacksaw/include/portage/package_config.h
        hacksaw/include/portage/portage.h
        hacksaw/include/portage/repository.h
        hacksaw/include/portage/use_flags.h
        hacksaw/include/tools/conf.h
        hacksaw/include/tools/debug.h
        hacksaw/include/tools/log.h
        hacksaw/include/tools/map.h
        hacksaw/include/tools/regular_expression.h
        hacksaw/include/tools/small_map.h
        hacksaw/include/tools/string.h
        hacksaw/include/tools/string_vector.h
        hacksaw/include/tools/vector.h
        hacksaw/include/tools/linked_vector.h
        hacksaw/include/tools/util.h
        )

set (language_SOURCES
        hacksaw/language/share.h
        hacksaw/language/share.c
        hacksaw/language/atom.c
        hacksaw/language/atom.h
        hacksaw/language/depend.c
        hacksaw/language/depend.h
        )

set (language_PARSER
        hacksaw/language/atom_bison.y
        hacksaw/language/atom_flex.l
        hacksaw/language/depend_bison.y
        hacksaw/language/depend_flex.l
        )

add_library (hacksaw SHARED
        ${hacksaw_HEADERS}
        ${hacksaw_SRC}
        ${language_BIN}
        ${language_PARSER}
        ${language_SOURCES}
        )

add_executable (hacksaw-test EXCLUDE_FROM_ALL hacksaw/src/main.c)
add_test (hacksaw-test hacksaw-test)
target_link_libraries (hacksaw-test hacksaw pcre2-8)

find_library(ARCHIVE archive)

set (LOCALSTATEDIR /var)
add_definitions (-DDBPATH="test")

add_executable (autogentoo-build ${arch_SOURCES} ${arch_HEADERS})
add_executable (autogentoo-daemon
        ${service_SOURCES}
        ${include_HEADERS})

TARGET_LINK_LIBRARIES(autogentoo-daemon curl ${ARCHIVE} pcre2-8 hacksaw)
TARGET_LINK_LIBRARIES(autogentoo-build curl ${ARCHIVE} pcre2-8 hacksaw)
