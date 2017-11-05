AM_CFLAGS = ${HACKSAW_CFLAGS} -I$(top_srcdir)/include/
ACLOCAL_AMFLAGS='-I m4'

AM_YFLAGS = -d

DEPENDFILES =   depend.c \
                depend_bison.y \
                depend_flex.l

PACKAGEFILES =  package.c \
                package_bison.y \
                package_flex.l

lib_LTLIBRARIES=libhacksaw-languages.la
libhacksaw_languages_la_LDFLAGS=-shared
libhacksaw_languages_la_SOURCES = $(PACKAGEFILES) $(DEPENDFILES) share.c

depend_flex.c: depend_flex.l
	$(LEX) -o depend_flex.c depend_flex.l

package_flex.c: package_flex.l
	$(LEX) -o package_flex.c package_flex.l

noinst_HEADERS = depend.h share.h
BUILT_SOURCES = depend_bison.h package_bison.h

DEPEND_CLEAN=depend_bison.h depend_bison.c depend_flex.c
PACKAGE_CLEAN=package_bison.h package_bison.c package_flex.c
CLEANFILES = *~ $(DEPEND_CLEAN) $(PACKAGE_CLEAN)

check_PROGRAMS = test-depend test-package
test_depend_SOURCES = depend-main.c
test_depend_LDADD = libhacksaw-languages.la ../src/libhacksaw.la
test_depend_CFLAGS  = ${AM_CFLAGS} -Wno-unused-result -g

test_package_SOURCES = package-main.c
test_package_LDADD = libhacksaw-languages.la ../src/libhacksaw.la
test_package_CFLAGS  = ${AM_CFLAGS} -Wno-unused-result -g