#define _GNU_SOURCE

#include <autogentoo/autogentoo.h>
#include <string.h>

Opt opt_handlers[] = {
		{'s', "server",  "Start the autogentoo server (instead of client)", set_is_server,      OPT_SHORT | OPT_LONG},
		{'r', "root",    "Set the root directory of the server",            set_location,       OPT_SHORT | OPT_LONG |
		                                                                                        OPT_ARG},
		{'h', "help",    "Print the help message and exit",                 print_help_wrapper, OPT_SHORT | OPT_LONG},
		{0,   "port",    "Set the port to bind the server to",              set_port,           OPT_LONG | OPT_ARG},
		{0,   "debug",   "Turn on the debug feature (nothing right now)",   set_debug,          OPT_LONG},
		{0,   "daemon",  "Run in background and detach from the terminal",  set_daemon,         OPT_LONG},
		{0,   "log",     "Pipe output to logfile",                          pipe_to_log,        OPT_LONG | OPT_ARG},
		{'t', "target",  "Target server (localhost default)",               set_target,         OPT_SHORT | OPT_LONG |
		                                                                                        OPT_ARG},
		{0,   "encrypt", "Start an encrypted socket as well",               set_is_encrypted,   OPT_LONG},
		{0,   "cert",    "speificy certificate file",                       set_encrypt_opts,   OPT_LONG | OPT_ARG},
		{0,   "rsa",     "speificy rsa private key file",                   set_encrypt_opts,   OPT_LONG | OPT_ARG},
		{0,   "sign",    "sign the rsa key with the certificate",           set_encrypt_opts,   OPT_LONG},
		{0,   "gencert", "generate new certicaite and self sign",           set_encrypt_opts,   OPT_LONG},
		{0,   "genrsa",  "generate new rsa and sign with cert",             set_encrypt_opts,   OPT_LONG},
		{0, NULL, NULL, NULL, (opt_opts_t) 0 }
};

static char* location = NULL;
static char port[5] = AUTOGENTOO_PORT;
static server_t server_opts;
static int logfile_fd = -1;
static char is_server = 0;
static char* client_opts = NULL;

static char* cert_file = NULL;
static char* rsa_file = NULL;
static enc_server_t enc_server_options = 0;

void set_encrypt_opts (Opt* op, char* arg) {
	if (strcmp(op->_long, "cert") == 0) {
		enc_server_options |= ENC_READ_CERT;
		cert_file = strdup(arg);
	}
	if (strcmp(op->_long, "rsa") == 0) {
		enc_server_options |= ENC_READ_RSA;
		rsa_file = strdup(arg);
	}
	if (strcmp(op->_long, "sign") == 0)
		enc_server_options |= ENC_CERT_SIGN;
	if (strcmp(op->_long, "gencert") == 0)
		enc_server_options |= ENC_GEN_CERT;
	if (strcmp(op->_long, "genrsa") == 0)
		enc_server_options |= ENC_GEN_RSA;
}

void set_is_encrypted (Opt* op, char* c) {
	server_opts |= ENCRYPT;
}

void set_is_server(Opt* op, char* c) {
	is_server = 1;
}

void set_target(Opt* op, char* target) {
	client_opts = strdup(target);
}

void set_location(Opt* op, char* loc) {
	location = strdup(loc);
	chdir(location);
}

void print_help_wrapper(Opt* op, char* arg) {
	print_help(opt_handlers);
	exit(0);
}

void set_port(Opt* op, char* c) {
	strcpy(port, c);
}

void set_debug(Opt* op, char* c) {
	server_opts |= DEBUG;
}

void set_daemon(Opt* op, char* c) {
	server_opts |= DAEMON;
}

void pipe_to_log(Opt* op, char* logfile) {
	FILE* f = fopen(logfile, "w+");
	lset(f);
}

int main(int argc, char** argv) {
	char* test = strdup("christina");
	print_bin(test, 1, sizeof(test));
	
	
	return 0;
	opt_handle(opt_handlers, argc, argv + 1);
	if (!location)
		location = strdup(".");
	if (!client_opts)
		client_opts = "";
	
	if (is_server) {
		Server* main_server = read_server(location, port, server_opts);
		main_server->keep_alive = 1;
		
		if (main_server->opts & ENCRYPT) {
			main_server->rsa_child = server_encrypt_new(main_server, AUTOGENTOO_PORT_ENCRYPT, cert_file, rsa_file, enc_server_options);
			if (!main_server->rsa_child) {
				server_free(main_server);
				lerror("Failed to initialize server");
				exit(1);
			}
		}
		
		if (main_server->opts & ENCRYPT)
			pthread_create(
					&main_server->rsa_child->pid,
					NULL,
					(void* (*)(void*))server_encrypt_start,
					main_server->rsa_child);
		server_start(main_server);
	}
	else {
		char* cmd;
		asprintf(&cmd, "python " AUTOGENTOO_CLIENT " %s", client_opts);
		system(cmd);
		free(cmd);
	}
	
	/* Exit sequence */
	free(location);
	if (logfile_fd != -1)
		close(logfile_fd);
	return 0;
	
}
