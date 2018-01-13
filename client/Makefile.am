AM_CFLAGS = ${AUTOGENTOO_CFLAGS} -I$(top_srcdir)/include/ -Wno-unused-result -I/src
ACLOCAL_AMFLAGS='-I m4'

noinst_LTLIBRARIES = libautogentoo-client.a
libautogentoo_client_a_SOURCES = src/interface.c
libautogentoo_client_la_CFLAGS = ${AM_CFLAGS}
libautogentoo_client_la_LDFLAGS = -version-info 0
