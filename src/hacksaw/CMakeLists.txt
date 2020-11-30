find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)

set(hacksaw_SRC
        debug.c
        log.c
        map.c
        small_map.c
        string_vector.c
        vector.c
        linked_vector.c
        util.c
        getopt.c
        set.c
        stack.c
        queue.c
        queue_set.c
        )

set(hacksaw_HEADERS
        hacksaw.h
        debug.h
        log.h
        map.h
        small_map.h
        string_vector.h
        vector.h
        linked_vector.h
        util.h
        getopt.h
        set.h
        stack.h
        queue.h
        )

add_library(hacksaw STATIC
        ${hacksaw_HEADERS}
        ${hacksaw_SRC})
