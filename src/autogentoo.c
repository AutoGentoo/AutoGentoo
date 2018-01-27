#include <autogentoo/autogentoo.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

Opt opt_handlers[] = {
        {'s', "server", "Start the autogentoo server (instead of client)", set_is_server, OPT_SHORT | OPT_LONG},
        {'r', "root", "Set the root directory of the server", set_location, OPT_SHORT | OPT_LONG | OPT_ARG},
        {'h', "help", "Print the help message and exit", print_help_wrapper, OPT_SHORT | OPT_LONG},
        {0, "port", "Set the port to bind the server to", set_port, OPT_LONG | OPT_ARG},
        {0, "debug", "Turn on the debug feature (nothing right now)", set_debug, OPT_LONG},
        {0, "daemon", "Run in background and detach from the terminal", set_daemon, OPT_LONG},
        {0, "log", "Pipe output to logfile", pipe_to_log, OPT_LONG | OPT_ARG},
        {0, NULL, NULL, NULL, 0}
};

static char* location = NULL;
static char port[5] = AUTOGENTOO_PORT;
static server_t server_opts;
static int logfile_fd = -1;
static char is_server = 0;

void set_is_server (Opt* op, char* c) {
    is_server = 1;
}

void set_location (Opt* op, char* loc) {
    location = strdup (loc);
    chdir (location);
}

void print_help_wrapper (Opt* op, char* arg) {
    print_help (opt_handlers);
    exit (0);
}

void set_port (Opt* op, char* c) {
    strcpy(port, c);
}

void set_debug (Opt* op, char* c) {
    server_opts |= DEBUG;
}

void set_daemon (Opt* op, char* c) {
    server_opts |= DAEMON;
}

void pipe_to_log (Opt* op, char* logfile) {
    FILE* f = fopen (logfile, "w+");
    lset (f);
}

int main (int argc, char** argv) {
    opt_handle (opt_handlers, argc, argv + 1);
    if (location == NULL)
        location = strdup(".");
    
    if (is_server) {
        Server* main_server = read_server (location, port, server_opts);
        server_start (main_server);
    }
    else {
        WindowManager* wmanager = init_manager ();
        render (wmanager);
        window_manager_free (wmanager);
    }
    
    /* Exit sequence */
    free (location);
    if (logfile_fd != -1)
        close (logfile_fd);
    return 0;
}
