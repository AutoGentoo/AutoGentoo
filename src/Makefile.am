AM_CFLAGS = ${HACKSAW_CFLAGS} -I$(top_srcdir)/include/
ACLOCAL_AMFLAGS='-I m4'

bin_PROGRAMS = hacksaw
hacksaw_SOURCES = main.c
hacksaw_CFLAGS = ${HACKSAW_CFLAGS} -I$(top_srcdir)/include/  -Wno-unused-result
hacksaw_LDADD = libhacksaw.la

lib_LTLIBRARIES = libhacksaw.la
libhacksaw_la_SOURCES = conf.c \
                        log.c  \
                        main.c \
                        manifest.c \
                        regular_expression.c  \
                        repository.c  \
                        string.c  \
                        string_vector.c  \
                        vector.c
libhacksaw_la_CFLAGS = ${HACKSAW_CFLAGS} -I$(top_srcdir)/include/  -Wno-unused-result
libhacksaw_la_LDFLAGS = -version-info 0
