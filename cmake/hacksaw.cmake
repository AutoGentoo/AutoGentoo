find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)

set(hacksaw_SRC
        src/hacksaw/conf.c
        src/hacksaw/debug.c
        src/hacksaw/log.c
        src/hacksaw/map.c
        src/hacksaw/small_map.c
        src/hacksaw/string_vector.c
        src/hacksaw/vector.c
        src/hacksaw/linked_vector.c
        src/hacksaw/util.c
        src/hacksaw/getopt.c
        src/hacksaw/set.c
        src/hacksaw/stack.c
        src/hacksaw/queue.c
        src/hacksaw/queue_set.c
        )

set(hacksaw_HEADERS
        include/hacksaw/hacksaw.h
        include/hacksaw/conf.h
        include/hacksaw/debug.h
        include/hacksaw/log.h
        include/hacksaw/map.h
        include/hacksaw/small_map.h
        include/hacksaw/string_vector.h
        include/hacksaw/vector.h
        include/hacksaw/linked_vector.h
        include/hacksaw/util.h
        include/hacksaw/getopt.h
        include/hacksaw/set.h
        include/hacksaw/stack.h
        include/hacksaw/queue.h
        )

