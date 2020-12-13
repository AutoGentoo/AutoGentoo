add_library(autogentoo_hacksaw SHARED
        hacksaw.h
        log.c log.h
        map.c map.h
        small_map.c small_map.h
        string_vector.c string_vector.h
        vector.c vector.h
        linked_vector.c linked_vector.h
        util.c util.h
        set.c set.h
        stack.c stack.h
        queue.c queue.h
        queue_set.c queue_set.h
        object.h
        global.h
        lut.c lut.h)

LinkPythonLib(autogentoo_hacksaw)
