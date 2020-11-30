find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)

add_library(hacksaw STATIC
        hacksaw.h
        log.c
        map.c
        small_map.c
        string_vector.c
        vector.c
        linked_vector.c
        util.c
        set.c
        stack.c
        queue.c
        queue_set.c
        log.h
        map.h
        small_map.h
        string_vector.h
        vector.h
        linked_vector.h
        util.h
        set.h
        stack.h
        queue.h
        object.h
        global.h)
