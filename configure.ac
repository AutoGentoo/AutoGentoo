AC_INIT([autogentoo], [5.0], [])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AC_PROG_CC
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])

AC_PROG_CXX
AC_PROG_CC
AM_PROG_AR
LT_INIT
AC_SUBST([LIBTOOL_DEPS])

#AC_CHECK_LIB([parted], [ped_device_read], [], AC_MSG_ERROR([*** libparted not found.]))

PKG_CHECK_MODULES(libcurl, libcurl)
#PKG_CHECK_MODULES(glib, glib-2.0)

#AC_ARG_ENABLE([gtk],
#  [AS_HELP_STRING([--disable-gtk], [Disable GTK support])],
#  [enable_gtk="no"],
#  [enable_gtk="yes"])

#AS_IF([test "$enable_gtk" != "no"],
#  [PKG_CHECK_MODULES([gtk],
#    [gtk+-3.0],
#    [enable_gtk=yes, PKG_CHECK_MODULES([GTK], [gtk+-3.0])],
#    [AS_IF([test "$enable_gtk" = "yes"],
#      [AC_MSG_ERROR([gtk+-3.0 required, but not found.])],
#      [enable_gtk=no])])])

#LIBPARTED_REQUIRED_VERSION='1.7.1'
#LIBPARTED_REQUIRED_INT=`echo "$LIBPARTED_REQUIRED_VERSION" |
#	$AWK -F. '{print $1 * 1000000 + $2 * 10000 + $3}'`
#dnl 1) Check using pkg-config.

#PKG_CHECK_EXISTS(
#  [libparted],
#  [LIBPARTED_FOUND_VERSION=`$PKG_CONFIG --modversion libparted`
#    LIBPARTED_FOUND_INT=`echo "$LIBPARTED_FOUND_VERSION" |
#    $AWK -F. '{print $1 * 1000000 + $2 * 10000 + $3}'`
#    AC_MSG_RESULT([$LIBPARTED_FOUND_VERSION])
#    test "$LIBPARTED_FOUND_INT" -ge "$LIBPARTED_REQUIRED_INT" ||
#    AC_MSG_ERROR([*** libparted too old.  Require libparted >= $LIBPARTED_REQUIRED_VERSION but only found libparted $LIBPARTED_FOUND_VERSION.])
#  ])

#AM_PATH_PYTHON([3.4],, [:])
#AM_CONDITIONAL([HAVE_PYTHON], [test "$PYTHON" != :])
#PY_PREFIX=`$PYTHON -c "import sys; print (sys.prefix)"`
#Y_INCLUDE=`$PYTHON -c "from distutils import sysconfig; print (sysconfig.get_python_inc())"`
#PY_LIB=`$PYTHON -c "from distutils import sysconfig; print (sysconfig.get_python_inc()[[sysconfig.get_python_inc().rfind('/')+1:]])"`
#PYTHON_LIBS="-l$PY_LIB"
#PYTHON_CFLAGS="-I$PY_INCLUDE"
#AC_SUBST([PYTHON_LIBS])
#AC_SUBST([PYTHON_CFLAGS])

AC_CONFIG_FILES([
  Makefile
  src/Makefile
  include/Makefile
])


AC_OUTPUT
