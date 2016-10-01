SUBDIRS = . tools
AM_CPPFLAGS = -I$(top_srcdir)/lib/
AM_CFLAGS = $(AUTOGENTOO_CFLAGS) ${libcurl_CFLAGS}
ACLOCAL_AMFLAGS='-I m4'
bin_PROGRAMS = autogentoo
autogentoo_SOURCES = autogentoo.c devices.c disk.c download.c mount.c profile.c stage3.c
autogentoo_LDADD = tools/libagcoretools.la ${libcurl_LIBS}
