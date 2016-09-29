AC_INIT([autogentoo], [4.2], [])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AC_PROG_CC
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIRS([m4])

AC_PROG_CXX
AC_PROG_CC
AM_PROG_AR
LT_INIT
AC_SUBST([LIBTOOL_DEPS])


AC_CHECK_LIB([parted], [ped_device_read], [], AC_MSG_ERROR([*** libparted not found.]))

PKG_CHECK_MODULES([AUTOGENTOO], [
    glib-2.0
    gtk+-3.0])

LIBPARTED_REQUIRED_VERSION='3.0'
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
])


AC_OUTPUT
