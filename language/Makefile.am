AM_CFLAGS = ${HACKSAW_CFLAGS} -I$(top_srcdir)/include/
ACLOCAL_AMFLAGS='-I m4'

AM_YFLAGS = -d

lib_LTLIBRARIES=libhacksaw-languages.la
libhacksaw_languages_la_LDFLAGS=-shared
libhacksaw_languages_la_SOURCES = depend.c \
                        depend_bison.y \
                        depend_flex.l

depend_flex.c: depend_flex.l
	$(LEX) -o depend_flex.c depend_flex.l

noinst_HEADERS = depend.h
BUILT_SOURCES = depend_bison.h

USE_CLEAN=depend_bison.h depend_bison.c depend_flex.c
CLEANFILES = *~ $(USE_CLEAN)
