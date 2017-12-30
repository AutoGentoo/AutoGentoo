AM_CFLAGS = ${HACKSAW_CFLAGS} -I$(top_srcdir)/include/ $(CFLAGS)
ACLOCAL_AMFLAGS='-I m4'

AM_YFLAGS = -d

DEPENDFILES =   depend.c \
                depend_bison.y \
                depend_flex.l

ATOMFILES =  atom.c \
                atom_bison.y \
                atom_flex.l

lib_LTLIBRARIES=libhacksaw-languages.la
libhacksaw_languages_la_LDFLAGS=-shared
libhacksaw_languages_la_SOURCES = $(ATOMFILES) $(DEPENDFILES) share.c

depend_flex.c: depend_flex.l
	$(LEX) -o depend_flex.c depend_flex.l

atom_flex.c: atom_flex.l
	$(LEX) -o atom_flex.c atom_flex.l

BUILT_SOURCES = depend_bison.h atom_bison.h

DEPEND_CLEAN=depend_bison.h depend_bison.c depend_flex.c
ATOM_CLEAN=atom_bison.h atom_bison.c atom_flex.c
CLEANFILES = *~ $(DEPEND_CLEAN) $(ATOM_CLEAN)

check_PROGRAMS = test-depend test-atom
test_depend_SOURCES = depend-main.c
test_depend_LDADD = libhacksaw-languages.la ../src/libhacksaw.la
test_depend_CFLAGS  = ${AM_CFLAGS} -Wno-unused-result -g

test_atom_SOURCES = atom-main.c
test_atom_LDADD = libhacksaw-languages.la ../src/libhacksaw.la
test_atom_CFLAGS  = ${AM_CFLAGS} -Wno-unused-result -g
