AC_INIT([autogentoo], [1.0], [])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AC_PROG_CC
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])

AM_PROG_AR
AC_PROG_YACC
AC_PROG_LEX
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

AC_CHECK_LIB(pcre2-8, pcre2_compile_8,
[
    AC_CHECK_LIB(pcre2-8, pcre2_match_data_create_8)
    AC_CHECK_LIB(pcre2-8, pcre2_match_8)
    AC_CHECK_LIB(pcre2-8, pcre2_get_error_message_8)
    AC_CHECK_LIB(pcre2-8, pcre2_get_ovector_pointer_8)
    AC_CHECK_LIB(pcre2-8, pcre2_match_data_free_8)
    AC_CHECK_LIB(pcre2-8, pcre2_code_free_8)
],
[
    AC_MSG_ERROR([Could not found the PCRE2 library])
])

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
    hacksaw/Makefile
    hacksaw/src/Makefile
    hacksaw/language/Makefile
    hacksaw/include/Makefile
    hacksaw/include/portage/Makefile
    hacksaw/include/tools/Makefile
    Makefile
    service/Makefile
    include/Makefile
])


AC_OUTPUT
