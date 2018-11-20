AM_CFLAGS = ${AUTOGENTOO_CFLAGS} -I$(top_srcdir)/include/ ${CFLAGS} -Wno-unused-result -I$(top_srcdir)/hacksaw/language/ -fPIC
ACLOCAL_AMFLAGS='-I m4'

noinst_LIBRARIES = libhacksaw.a
libhacksaw_a_CFLAGS  =      ${AM_CFLAGS}
libhacksaw_a_LIBADD  =      ../language/liblanguage.a
libhacksaw_a_SOURCES =      dependency.c \
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
                            small_map.c \
                            string.c \
                            string_vector.c \
                            vector.c \
                            linked_vector.c \
                            util.c \
                            server_loop.c

bin_PROGRAMS    =   hacksaw
hacksaw_CFLAGS  =   ${AM_CFLAGS}
hacksaw_SOURCES =   main.c
hacksaw_LDADD   =   libhacksaw.a ../language/liblanguage.a
