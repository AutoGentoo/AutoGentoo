#include <autogentoo.h>
#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

Opt opt_handlers[] = {
        {'r', "root", "Set the root directory of the server", set_location, OPT_SHORT | OPT_LONG | OPT_ARG},
        {'h', "help", "Print the help message and exit", print_help_wrapper, OPT_SHORT | OPT_LONG},
        {0, "port", "Set the port to bind the server to", set_port, OPT_LONG | OPT_ARG},
        {0, "debug", "Turn on the debug feature (nothing right now)", set_debug, OPT_LONG},
        {0, "daemon", "Run in background and detach from the terminal", set_daemon, OPT_LONG},
        {0, NULL, NULL, NULL, 0}
};

static char* location = NULL;
static int port = AUTOGENTOO_PORT;
static server_t server_opts;

void set_location (Opt* op, char* loc) {
    location = strdup (loc);
}

void print_help_wrapper (Opt* op, char* arg) {
    print_help (opt_handlers);
    exit (0);
}

void set_port (Opt* op, char* c) {
    port = atoi(c);
}

void set_debug (Opt* op, char* c) {
    server_opts |= DEBUG;
}

void set_daemon (Opt* op, char* c) {
    server_opts |= DAEMON;
}

int main (int argc, char** argv) {
    opt_handle (opt_handlers, argc, argv + 1);
    if (location == NULL)
        location = strdup(".");
    
    Server* main_server = read_server (location, port, server_opts);
    
    free (location);
    server_start (main_server);
}
