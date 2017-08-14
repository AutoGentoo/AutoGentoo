AM_CFLAGS = ${AUTOGENTOO_CFLAGS} ${PYTHON_CFLAGS} -I$(top_srcdir)/include/ $(PYTHON_LIBS)
ACLOCAL_AMFLAGS='-I m4'

#check_PROGRAMS = testlibip testipscan
#testlibip_SOURCES = testhtoip.c
#testlibip_LDADD = libip.la

#testipscan_SOURCES = ipscantest.c ip_scan.c
#testipscan_LDADD = libip.la

check_PROGRAMS = testserveclient testserver
testserveclient_SOURCES = testserveclient.c
testserveclient_LDADD = libip.la libagserver.la

testserver_SOURCES = testserver.c
testserver_LDADD = libagserver.la

lib_LTLIBRARIES = libip.la libagserver.la
libip_la_SOURCES = ip_convert.c ip_scan.c
libip_la_CFLAGS = ${AUTOGENTOO_CFLAGS} ${PYTHON_CFLAGS} -I$(top_srcdir)/include/
libip_la_LDFLAGS = -version-info 0

libagserver_la_SOURCES = serve_client.c list.c make_conf.c server.c
libagserver_la_CFLAGS = ${AUTOGENTOO_CFLAGS} ${PYTHON_CFLAGS} -I$(top_srcdir)/include/
libagserver_la_LDFLAGS = -version-info 0
