AM_CFLAGS = ${AUTOGENTOO_CFLAGS} ${PYTHON_CFLAGS} -I$(top_srcdir)/include/
ACLOCAL_AMFLAGS='-I m4'

lib_LTLIBRARIES = libip.la
libip_la_SOURCES = htoip_port.c
libip_la_CFLAGS = ${AUTOGENTOO_CFLAGS} ${PYTHON_CFLAGS} -I$(top_srcdir)/include/
libip_la_LDFLAGS = -module -avoid-version -export-symbols-regex initmyext
