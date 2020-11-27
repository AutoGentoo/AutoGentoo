//
// Created by atuser on 4/23/19.
//

#define _GNU_SOURCE

#include <time.h>
#include <stdarg.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <autogentoo/hacksaw/log.h>
#include <autogentoo/hacksaw/hacksaw.h>
#include "constants.h"
#include "package.h"
#include "database.h"

static char log_time_buffer[64];
static StringVector* portage_call_stack;

int debug = 0;

void plog_init() {
    portage_call_stack = string_vector_new();
}

void plog_enter_stack(char* stack_name, ...) {
    va_list ap;
    va_start(ap, stack_name);
    char* stack_parsed = NULL;
    vasprintf(&stack_parsed, stack_name, ap);
    string_vector_add(portage_call_stack, stack_parsed);
    va_end(ap);
    free(stack_parsed);
}

void plog_exit_stack() {
    string_vector_remove(portage_call_stack, portage_call_stack->n - 1);
}

void plog_format_time() {
    time_t rawtime;
    struct tm* info;
    rawtime = time(NULL);
    info = localtime(&rawtime);

    strftime(log_time_buffer, 64, "%c", info);
}

void plog_info(char* fmt, ...) {
    plog_format_time();
    printf("[cportage - %s] INFO -- ", log_time_buffer);
    va_list ap;
    va_start(ap, fmt);
    vprintf(fmt, ap);
    printf("\n");
    va_end(ap);
}

void plog_debug(char* fmt, ...) {
    if (!debug)
        return;

    plog_format_time();
    printf("[cportage - %s] DEBUG -- ", log_time_buffer);
    va_list ap;
    va_start(ap, fmt);
    vprintf(fmt, ap);
    printf("\n");
    va_end(ap);
}

void plog_error(char* fmt, ...) {
    int buff_error = errno;
    plog_format_time();
    printf("[cportage - %s] ERROR -- ", log_time_buffer);
    va_list ap;
    va_start(ap, fmt);
    vprintf(fmt, ap);
    va_end(ap);
    printf("\n");
    if (errno != 0)
        printf("[cportage - %s] ERROR -- %s (%d)\n", log_time_buffer, strerror(buff_error), buff_error);
}

void plog_warn(char* fmt, ...) {
    plog_format_time();
    printf("[cportage - %s] WARN -- ", log_time_buffer);
    va_list ap;
    va_start(ap, fmt);
    vprintf(fmt, ap);
    va_end(ap);
    printf("\n");
}

void portage_die(char* fmt, ...) {
    FILE* target = stderr;
    va_list args;
    va_start (args, fmt);
    fprintf(target, ANSI_BOLD ANSI_RED " * " ANSI_COLOR);
    vfprintf(target, fmt, args);
    va_end (args);
    fprintf(target, ANSI_RESET "\n");
    fprintf(target, ANSI_BOLD ANSI_RED " * " ANSI_COLOR "PORTAGE CALL STACK:\n");
    for (int i = 0; i < portage_call_stack->n; i++) {
        fprintf(target, "%*c%s\n", (i * 2) % 30, ' ', string_vector_get(portage_call_stack, i));
    }
    exit(1);
}

void resolved_ebuild_print(Emerge* em, ResolvedEbuild* se) {
    printf("[ ");
    if (se->action & PORTAGE_NEW)
        printf("N");
    else
        printf(" ");

    if (se->action & PORTAGE_SLOT)
        printf("S");
    else
        printf(" ");

    if (se->action & PORTAGE_REPLACE)
        printf("R");
    else
        printf(" ");

    if (se->action & PORTAGE_UPDATE)
        printf("U");
    else if (se->action & PORTAGE_DOWNGRADE)
        printf("D");
    else
        printf(" ");

    if (se->action & PORTAGE_USE_FLAG)
        printf("F");
    else
        printf(" ");

    if (se->action & PORTAGE_BLOCK)
        printf("B");
    else
        printf(" ");

    printf(" ] %s", se->target->ebuild_key);

    printf(" ");
    for (UseFlag* use = se->use; use; use = use->next) {
        UseFlag* ebuild_use = NULL;
        use_t ebuild_use_status = USE_NONE;

        if (se->installed)
            ebuild_use = use_get(se->installed->use, use->name);

        if (ebuild_use)
            ebuild_use_status = ebuild_use->status;
        else if (!(em->options & EMERGE_VERBOSE))
            continue;

        if ((ebuild_use_status != use->status && se->installed) || em->options & EMERGE_VERBOSE) {
            if (use->status == USE_ENABLE)
                printf("\033[1;31m");
            else
                printf("\033[1;34m-");
            printf("%s%s\033[0m ", use->name, ebuild_use_status != use->status && ebuild_use ? "*" : "");
        }
    }

    if (se->installed) {
        printf(" %s", se->installed->version->full_version);
        if (se->installed->revision != 0)
            printf("-r%d", se->installed->revision);
    }

    printf("\n");
}