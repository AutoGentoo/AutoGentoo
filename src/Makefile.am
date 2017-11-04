AM_CFLAGS = ${HACKSAW_CFLAGS} -I$(top_srcdir)/include/
ACLOCAL_AMFLAGS='-I m4'

lib_LTLIBRARIES=libhacksaw.la
libhacksaw_la_CFLAGS = ${HACKSAW_CFLAGS} -I$(top_srcdir)/include/ -Wno-unused-result
libhacksaw_la_LDFLAGS=-shared
libhacksaw_la_SOURCES =    conf.c \
                        log.c  \
                        manifest.c \
                        regular_expression.c  \
                        repository.c  \
                        string.c  \
                        string_vector.c  \
                        vector.c \
                        debug.c \
                        package.c \
                        directory.c

bin_PROGRAMS = hacksaw
hacksaw_CFLAGS  = ${AM_CFLAGS} -Wno-unused-result -g
hacksaw_SOURCES =   main.c
hacksaw_LDADD   =   ../language/libhacksaw-languages.la libhacksaw.la