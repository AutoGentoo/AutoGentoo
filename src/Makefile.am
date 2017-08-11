AM_CFLAGS = ${AUTOGENTOO_CFLAGS} ${PYTHON_CFLAGS} -I$(top_srcdir)/include/ $(PYTHON_LIBS)
ACLOCAL_AMFLAGS='-I m4'

check_PROGRAMS = testlibip testipscan
testlibip_SOURCES = testhtoip.c
testlibip_LDADD = libip.la

testipscan_SOURCES = ipscantest.c ip_scan.c
testipscan_LDADD = libip.la

lib_LTLIBRARIES = libip.la
libip_la_SOURCES = ip_convert.c ip_scan.c
libip_la_CFLAGS = ${AUTOGENTOO_CFLAGS} ${PYTHON_CFLAGS} -I$(top_srcdir)/include/
libip_la_LDFLAGS = -version-info 0
