cmake_minimum_required(VERSION 2.8.9)
project(AutoGentoo)
include_directories(include)

SET(CMAKE_C_COMPILER gcc)

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
        arch/ini.c)

set (arch_HEADERS
        arch/aabs.h
        arch/db.h
        arch/package.h
        arch/repo.h
        arch/util.h
        arch/write.h
        arch/deps.h
        arch/ini.h
        )

find_library(ARCHIVE archive)

set (LOCALSTATEDIR /var)
add_definitions (-DDBPATH="test")

add_executable (autogentoo-build ${arch_SOURCES} ${arch_HEADERS})
add_executable (autogentoo-daemon
        ${service_SOURCES}
        ${include_HEADERS})

TARGET_LINK_LIBRARIES(autogentoo-daemon curl ${ARCHIVE} hacksaw)
TARGET_LINK_LIBRARIES(autogentoo-build curl ${ARCHIVE} hacksaw)
