AM_CFLAGS = ${HACKSAW_CFLAGS} -I$(top_srcdir)/include/ $(CFLAGS)
ACLOCAL_AMFLAGS='-I m4'

noinst_LIBRARIES=liblanguage.a
liblanguage_a_CFLAGS  = ${AM_CFLAGS} -fPIC
liblanguage_a_SOURCES = \
               atom.c \
               atom.tab.c \
               depend.c \
               depend.tab.c \
               share.c \
               atom.flex.c \
               depend.flex.c


atom.tab.h:
	$(MAKE) atom.tab.c

depend.tab.h:
	$(MAKE) depend.tab.c

atom.tab.c: atom_bison.y
	$(YACC) -d $^ -o $@ --defines=atom.tab.h

depend.tab.c: depend_bison.y
	$(YACC) -d $^ -o $@ --defines=depend.tab.h

atom.flex.c: atom_flex.l atom.tab.h
	$(LEX) -o $@ $<

depend.flex.c: depend_flex.l depend.tab.h
	$(LEX) -o $@ $<

BUILT_SOURCES = depend.tab.h atom.tab.h

DEPEND_CLEAN=depend.tab.h depend.tab.c depend.flex.c
ATOM_CLEAN=atom.tab.h atom.tab.c atom.flex.c
CLEANFILES = *~ $(DEPEND_CLEAN) $(ATOM_CLEAN)

check_PROGRAMS = test-depend test-atom
test_depend_SOURCES = depend-main.c
test_depend_LDADD = languages.a ../src/libhacksaw.la
test_depend_CFLAGS  = ${AM_CFLAGS} -Wno-unused-result -g

test_atom_SOURCES = atom-main.c
test_atom_LDADD = languages.a ../src/libhacksaw.la
test_atom_CFLAGS  = ${AM_CFLAGS} -Wno-unused-result -g
