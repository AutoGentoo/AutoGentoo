//
// Created by atuser on 10/22/17.
//

#include <stdio.h>
#include <tools/log.h>
#include <stdarg.h>

char* bash_codes[] = {
        "\\e[0",
        "\\e[1",
        "\\e[21",
        "\\e[39",
        "\\e[31",
        "\\e[32",
        "\\e[33",
        "\\e[34",
};

char* bash_code (bash_t code) {
    return bash_codes[(int)code];
}

void lerror (char* format, ...) {
    fprintf (stderr, "%s%s * ", bash_code(BASH_BOLD), bash_code(BASH_RED));va_list args;
    va_start (args, format);
    vfprintf (stderr, format, args);
    va_end (args);
    fprintf(stderr, "%s\n", bash_code(BASH_NORMAL));
}

void lwarning (char* format, ...) {
    printf ("%s%s * ", bash_code(BASH_BOLD), bash_code(BASH_YELLOW));va_list args;
    va_start (args, format);
    vprintf (format, args);
    va_end (args);
    printf("%s\n", bash_code(BASH_NORMAL));
}

void linfo (char* format, ...) {
    printf ("%s%s * ", bash_code(BASH_BOLD), bash_code(BASH_GREEN));va_list args;
    va_start (args, format);
    vprintf (format, args);
    va_end (args);
    printf("%s\n", bash_code(BASH_NORMAL));
}
