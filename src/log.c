//
// Created by atuser on 10/22/17.
//

#include <stdio.h>
#include <tools/log.h>
#include <stdarg.h>

void lerror (char* format, ...) {
    fprintf (stderr, ANSI_BOLD ANSI_RED " * ");
    va_list args;
    va_start (args, format);
    vfprintf (stderr, format, args);
    va_end (args);
    fprintf (stderr, ANSI_RESET "\n");
}

void lwarning (char* format, ...) {
    printf (ANSI_BOLD ANSI_YELLOW " * ");
    va_list args;
    va_start (args, format);
    vprintf (format, args);
    va_end (args);
    printf (ANSI_RESET "\n");
}

void linfo (char* format, ...) {
    printf (ANSI_BOLD ANSI_GREEN " * ");
    va_list args;
    va_start (args, format);
    vprintf (format, args);
    va_end (args);
    printf (ANSI_RESET "\n");
}
