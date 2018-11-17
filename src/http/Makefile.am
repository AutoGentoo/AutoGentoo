AM_CFLAGS = ${AUTOGENTOO_CFLAGS} -I$(top_srcdir)/include/ $(CFLAGS) -I$(top_srcdir)/src/http/
ACLOCAL_AMFLAGS='-I m4'

noinst_LIBRARIES=libhttp.a
libhttp_a_CFLAGS  = ${AM_CFLAGS} -fPIC
libhttp_a_SOURCES = \
               ag_http.tab.c \
               ag_http.flex.c


ag_http.tab.h:
	$(MAKE) ag_http.tab.c

ag_http.tab.c: ag_http_bison.y
	$(YACC) -d $^ -o $@ --defines=ag_http.tab.h

ag_http.flex.c: ag_http_flex.l ag_http.tab.h
	$(LEX) -o $@ $<

BUILT_SOURCES = ag_http.tab.h
CLEANFILES = ag_http.tab.h ag_http.tab.c ag_http.flex.c
