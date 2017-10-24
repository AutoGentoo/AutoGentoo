AM_CFLAGS = ${HACKSAW_CFLAGS} -I$(top_srcdir)/include/
ACLOCAL_AMFLAGS='-I m4'

bin_PROGRAMS = hacksaw
hacksaw_CFLAGS = ${HACKSAW_CFLAGS} -I$(top_srcdir)/include/  -Wno-unused-result
hacksaw_SOURCES =       main.c \
                        conf.c \
                        log.c  \
                        manifest.c \
                        regular_expression.c  \
                        repository.c  \
                        string.c  \
                        string_vector.c  \
                        vector.c