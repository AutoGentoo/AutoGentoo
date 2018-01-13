AM_CFLAGS = ${HACKSAW_CFLAGS} -I$(top_srcdir)/include/ -I$(top_srcdir)/ ${CFLAGS} -Wno-unused-result
ACLOCAL_AMFLAGS='-I m4'

lib_LTLIBRARIES=libhacksaw.la
libhacksaw_la_CFLAGS  =     ${AM_CFLAGS}
libhacksaw_la_LDFLAGS =     -shared
libhacksaw_la_LIBADD  =     ../language/liblanguage.a
libhacksaw_la_SOURCES =     dependency.c \
                            directory.c \
                            hash.c \
                            manifest.c \
                            package.c \
                            package_config.c \
                            repository.c \
                            use_flag.c \
                            conf.c \
                            debug.c \
                            log.c \
                            map.c \
                            regular_expression.c \
                            small_map.c \
                            string.c \
                            string_vector.c \
                            vector.c \
                            linked_vector.c \
                            util.c

#bin_PROGRAMS    =   hacksaw
#hacksaw_CFLAGS  =   ${AM_CFLAGS}
#hacksaw_SOURCES =   main.c
#hacksaw_LDADD   =   libhacksaw.la
