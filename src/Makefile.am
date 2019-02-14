AUTOMAKE_OPTIONS = subdir-objects
SUBDIRS = http . api
AM_CFLAGS = ${AUTOGENTOO_CFLAGS} -I$(top_srcdir)/include/ -Wno-unused-result ${CRYPT_KEY}

ACLOCAL_AMFLAGS='-I m4'

CRYPT_KEY_COMMAND=cat /dev/urandom | tr -dc 'a-zA-Z0-9' | fold -w 32 | head -n 1
lib_LTLIBRARIES = libautogentoo.la
libautogentoo_la_SOURCES =  \
	getopt.c          pool.c    response.c           stage.c \
	chroot.c          handle.c  queue.c              server.c         thread.c \
	crypt.c           host.c    server_handle.c      user.c           endian_convert.c  \
	http.c            request.c server_init.c        writeconfig.c    server_loop.c \
	gentoo_config.c   kernel.c  request_structure.c  http_handle.c  \
	api/dynamic_binary.c api/request_generate.c api/request_check.c \
	 ../hacksaw/src/libhacksaw.a http/libhttp.a

libautogentoo_la_CFLAGS = ${AM_CFLAGS} -DAUTOGENTOO_USER_AUTH_ENCRYPT=`${CRYPT_KEY_COMMAND}` -fPIC
libautogentoo_la_LDFLAGS = ../hacksaw/src/libhacksaw.a http/libhttp.a -version-info 0

bin_PROGRAMS = autogentoo
autogentoo_SOURCES = autogentoo.c
autogentoo_CFLAGS = ${AM_CFLAGS} -lpthread -D AUTOGENTOO_CLIENT=\"$(libdir)/autogentoo/client.py\"
autogentoo_LDADD = libautogentoo.la
