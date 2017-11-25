AC_INIT([autogentoo], [5.2], [])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AC_PROG_CC
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])

AC_PROG_CC
AM_PROG_AR
LT_INIT

AC_SUBST([LIBTOOL_DEPS])

AC_CHECK_LIB([hacksaw-tools], [vector_new])
AC_CHECK_LIB([hacksaw-tools], [string_vector_new])
AC_CHECK_LIB([hacksaw-tools], [map_new])
AC_CHECK_LIB([hacksaw-tools], [small_map_new])
AC_CHECK_LIB([hacksaw-tools], [conf_new])


AC_CONFIG_FILES([
  Makefile
  src/Makefile
  include/Makefile
])


AC_OUTPUT
