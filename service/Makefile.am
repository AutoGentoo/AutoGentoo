AM_CFLAGS = ${AUTOGENTOO_CFLAGS} -I$(top_srcdir)/include/ -Wno-unused-result  -I$(top_srcdir)/hacksaw/include/ -I$(top_srcdir)/hacksaw/
ACLOCAL_AMFLAGS='-I m4'

lib_LTLIBRARIES = libautogentoo.la
libautogentoo_la_SOURCES =  \
	handle.c response.c server.c host.c chroot.c \
	writeconfig.c stage.c download.c command.c \
	kernel.c endian_convert.c

libautogentoo_la_CFLAGS = ${AM_CFLAGS}
libautogentoo_la_LDFLAGS = -version-info 0

bin_PROGRAMS = autogentoo
autogentoo_SOURCES = autogentoo.c
autogentoo_CFLAGS = ${AM_CFLAGS}
autogentoo_LDADD = libautogentoo.la ../hacksaw/src/libhacksaw.a
