AC_INIT([hacksaw], [1.0], [])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AC_PROG_CC
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])

AC_PROG_CC
AM_PROG_AR
LT_INIT

AC_SUBST([LIBTOOL_DEPS])

AC_CHECK_LIB(pcre2-8, pcre2_compile_8)

# Checks for header files.
AC_CHECK_HEADERS([memory.h stddef.h stdlib.h string.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([memmove memset regcomp strtol])

AC_CONFIG_FILES([
  Makefile
  src/Makefile
  include/Makefile
  include/config/Makefile
  include/package/Makefile
  include/portage/Makefile
  include/test/Makefile
  include/tools/Makefile
])


AC_OUTPUT
