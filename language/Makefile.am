AM_CFLAGS = ${HACKSAW_CFLAGS} -I$(top_srcdir)/include/
ACLOCAL_AMFLAGS='-I m4'

AM_YFLAGS = -d
bin_PROGRAMS = hacksaw-uc

hacksaw_uc_SOURCES =    depend.c \
                        depend_bison.y \
                        depend_flex.l
hacksaw_uc_LDADD   =    ../src/libhacksaw.la

noinst_HEADERS = depend.h
BUILT_SOURCES = depend_bison.h lex.depend.c

USE_CLEAN=depend_bison.h depend_bison.c lex.depend.c
CLEANFILES = *~ $(USE_CLEAN)
