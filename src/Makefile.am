SUBDIRS = http ui
AM_CFLAGS = ${AUTOGENTOO_CFLAGS} -I$(top_srcdir)/include/ -Wno-unused-result ${CRYPT_KEY}

ACLOCAL_AMFLAGS='-I m4'

CRYPT_KEY_COMMAND=cat /dev/urandom | tr -dc 'a-zA-Z0-9' | fold -w 32 | head -n 1
lib_LTLIBRARIES = libautogentoo.la
libautogentoo_la_SOURCES =  \
	handle.c response.c server.c host.c chroot.c \
	writeconfig.c stage.c download.c command.c \
	kernel.c endian_convert.c getopt.c thread.c \
	request.c request_structure.c queue.c

libautogentoo_la_CFLAGS = ${AM_CFLAGS} -D$(CRYPT_KEY_COMMAND)
libautogentoo_la_LDFLAGS = ../hacksaw/src/libhacksaw.a http/libhttp.a -version-info 0

bin_PROGRAMS = autogentoo
autogentoo_SOURCES = autogentoo.c
autogentoo_CFLAGS = ${AM_CFLAGS} -D AUTOGENTOO_CLIENT=\"$(libdir)/autogentoo/client.py\"
autogentoo_LDADD = libautogentoo.la
