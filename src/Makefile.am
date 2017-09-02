AM_CFLAGS = ${AUTOGENTOO_CFLAGS} ${PYTHON_CFLAGS} -I$(top_srcdir)/include/ $(PYTHON_LIBS)
ACLOCAL_AMFLAGS='-I m4'

pkgdata_DATA = update.py

bin_PROGRAMS = autogentoo
autogentoo_SOURCES = autogentoo.c
autogentoo_CFLAGS = -D AUTOGENTOO_DATADIR=$(pkgdatadir) ${AUTOGENTOO_CFLAGS} -I$(top_srcdir)/include/
autogentoo_LDADD = libautogentoo.la

#check_PROGRAMS = testlibip testipscan
#testlibip_SOURCES = testhtoip.c
#testlibip_LDADD = libip.la

#testipscan_SOURCES = ipscantest.c ip_scan.c
#testipscan_LDADD = libip.la

check_PROGRAMS = testserveclient testserver
testserveclient_SOURCES = testserveclient.c
testserveclient_LDADD = libautogentoo.la

testserver_SOURCES = testserver.c
testserver_LDADD = libautogentoo.la

lib_LTLIBRARIES = libautogentoo.la
libautogentoo_la_SOURCES = serve_client.c list.c server.c response.c request.c emerge.c _string.c ip_convert.c ip_scan.c
libautogentoo_la_CFLAGS = ${AUTOGENTOO_CFLAGS} -I$(top_srcdir)/include/
libautogentoo_la_LDFLAGS = -version-info 0
