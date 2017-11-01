AM_CFLAGS = ${HACKSAW_CFLAGS} -I$(top_srcdir)/include/
ACLOCAL_AMFLAGS='-I m4'

AM_YFLAGS = -d
bin_PROGRAMS = hacksaw-uc

hacksaw_uc_SOURCES =    use.c \
                        use_bison.y \
                        use_flex.l

noinst_HEADERS = use.h
BUILT_SOURCES = use_bison.h

CLEANFILES = *~