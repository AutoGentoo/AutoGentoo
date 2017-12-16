AC_INIT([autogentoo], [1.0], [])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AC_PROG_CC
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])

AC_PROG_CC
AM_PROG_AR
LT_INIT

AC_SUBST([LIBTOOL_DEPS])

AC_CHECK_HEADERS([archive.h archive_entry.h], [], [
    AC_MSG_ERROR(["Missing libarchive headers"])
])

AC_CHECK_LIB([archive], [archive_read_new], [], [
    AC_MSG_ERROR(["libarchive could not be found"])
])

AC_CHECK_LIB([curl], [curl_easy_init], [], [
    AC_MSG_ERROR(["libcurl could not be found"])
])

AC_CHECK_LIB([hacksaw-tools], [vector_new], [], [
    AC_MSG_ERROR(["libhacksaw could not be found"])
])


AC_CONFIG_FILES([
  Makefile
  src/Makefile
  include/Makefile
])


AC_OUTPUT
