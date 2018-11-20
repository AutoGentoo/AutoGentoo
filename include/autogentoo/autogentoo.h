#ifndef __AUTOGENTOO_H__
#define __AUTOGENTOO_H__
#define _GNU_SOURCE

#include "hacksaw/hacksaw.h"
#include "chroot.h"
#include "command.h"
#include "download.h"
#include "endian_convert.h"
#include "getopt.h"
#include "handle.h"
#include "host.h"
#include "kernel.h"
#include "endian_convert.h"
#include "getopt.h"
#include "handle.h"
#include "host.h"
#include "response.h"
#include "server.h"
#include "thread.h"
#include "stage.h"
#include "writeconfig.h"
#include "render.h"
#include "crypt.h"

#ifndef AUTOGENTOO_LBIN_PATH
#define AUTOGENTOO_LBIN_PATH "/usr/lib/autogentoo/"
#endif

#ifndef AUTOGENTOO_CLIENT
#define AUTOGENTOO_CLIENT AUTOGENTOO_LBIN_PATH "client.py"
#endif

#ifndef AUTOGENTOO_WORKER
#define AUTOGENTOO_WORKER AUTOGENTOO_LBIN_PATH "worker.py"
#endif


/* Port to 9491 so we don't overlap with our main service */
//#define AUTOGENTOO_DEBUG

//#define AUTOGENTOO_NO_THREADS

#ifdef AUTOGENTOO_DEBUG
#define AUTOGENTOO_PORT "9490"
#define AUTOGENTOO_PORT_ENCRYPT "9491"
#else
#define AUTOGENTOO_PORT "80"
#define AUTOGENTOO_PORT_ENCRYPT "443"
#endif

void set_encrypt_opts (Opt* op, char* arg);

void set_is_encrypted (Opt* op, char* c);

void set_is_server(Opt* op, char* c);

void set_location(Opt* op, char* loc);

void print_help_wrapper(Opt* op, char* arg);

void set_port(Opt* op, char* c);

void set_debug(Opt* op, char* c);

void set_daemon(Opt* op, char* c);

void pipe_to_log(Opt* op, char* logfile);

void set_target(Opt* op, char* target);

int main(int argc, char** argv);

#endif