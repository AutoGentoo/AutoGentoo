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
AC_CHECK_LIB([ssl], [SSL_library_init])
AC_CHECK_LIB([crypto], [SHA256_Init])
AC_CHECK_LIB([crypto], [SHA256_Final])
AC_CHECK_LIB([crypto], [SHA256_Update])


AC_CONFIG_FILES([
  Makefile
  src/Makefile
  include/Makefile
])


AC_OUTPUT
