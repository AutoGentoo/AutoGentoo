AC_INIT([autogentoo], [4.2], [])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AC_PROG_CC
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])

AC_PROG_CXX
AC_PROG_CC
AM_PROG_AR
LT_INIT
AC_SUBST([LIBTOOL_DEPS])

AC_CHECK_LIB([parted], [ped_device_read], [], AC_MSG_ERROR([*** libparted not found.]))

PKG_CHECK_MODULES(libcurl, libcurl)
PKG_CHECK_MODULES(glib, glib-2.0)

AC_ARG_ENABLE([gtk],
  [AS_HELP_STRING([--disable-gtk], [Disable GTK support])],
  [enable_gtk="no"],
  [enable_gtk="yes"])

AS_IF([test "$enable_gtk" != "no"],
  [PKG_CHECK_MODULES([gtk],
    [gtk+-3.0],
    [enable_gtk=yes, PKG_CHECK_MODULES([GTK], [gtk+-3.0])],
    [AS_IF([test "$enable_gtk" = "yes"],
      [AC_MSG_ERROR([gtk+-3.0 required, but not found.])],
      [enable_gtk=no])])])

LIBPARTED_REQUIRED_VERSION='1.7.1'
LIBPARTED_REQUIRED_INT=`echo "$LIBPARTED_REQUIRED_VERSION" |
	$AWK -F. '{print $1 * 1000000 + $2 * 10000 + $3}'`
dnl 1) Check using pkg-config.

PKG_CHECK_EXISTS(
  [libparted],
  [LIBPARTED_FOUND_VERSION=`$PKG_CONFIG --modversion libparted`
    LIBPARTED_FOUND_INT=`echo "$LIBPARTED_FOUND_VERSION" |
    $AWK -F. '{print $1 * 1000000 + $2 * 10000 + $3}'`
    AC_MSG_RESULT([$LIBPARTED_FOUND_VERSION])
    test "$LIBPARTED_FOUND_INT" -ge "$LIBPARTED_REQUIRED_INT" ||
    AC_MSG_ERROR([*** libparted too old.  Require libparted >= $LIBPARTED_REQUIRED_VERSION but only found libparted $LIBPARTED_FOUND_VERSION.])
  ])

AC_CONFIG_FILES([
  Makefile
  src/Makefile
  src/core/Makefile
  src/core/tools/Makefile
  src/http/Makefile
  src/portage/Makefile
  src/gui/Makefile
  lib/Makefile
  lib/http/Makefile
  lib/portage/Makefile
  lib/gui/Makefile
  lib/core/Makefile
  lib/core/tools/Makefile
])


AC_OUTPUT
