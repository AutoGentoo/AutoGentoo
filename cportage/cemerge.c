//
// Created by atuser on 4/23/19.
//


#include <openssl/evp.h>
#include <share.h>
#include <autogentoo/hacksaw/hacksaw.h>
#include <autogentoo/hacksaw/getopt.h>
#include "portage.h"
#include "portage_log.h"
#include "manifest.h"
#include "emerge.h"
#include "database.h"
#include <string.h>
#include <errno.h>

Emerge* emerge_main;

void set_quiet(Opt* opt, char* arg) {
	emerge_main->options |= EMERGE_QUIET;
}

void set_deep(Opt* opt, char* arg) {
	emerge_main->options |= EMERGE_DEEP;
}

void set_root(Opt* opt, char* arg) {
	emerge_main->options |= EMERGE_BUILDROOT;
	if (emerge_main->root)
		free(emerge_main->root);
	emerge_main->root = strdup(arg);
}

void set_installroot(Opt* opt, char* arg) {
	emerge_main->options |= EMERGE_INSTALLROOT;
	if (emerge_main->installroot)
		free(emerge_main->installroot);
	emerge_main->installroot = strdup(arg);
}

void set_arch(Opt* opt, char* arg) {
	emerge_main->target_arch = get_arch(arg);
	if (emerge_main->target_arch == ARCH_END) {
		lerror("Invalid arch %s", arg);
		exit(1);
	}
}

void usepkg(Opt* op, char* arg) {
	emerge_main->build_opts |= EMERGE_USE_BINHOST;
}

void verbose(Opt* op, char* arg) {
	emerge_main->options |= EMERGE_VERBOSE;
}

void print_help_wrapper(Opt* op, char* arg);

Opt opt_handlers[] = {
		{'q', "quiet",  "Don't print the build logs during emerge", set_quiet, OPT_SHORT | OPT_LONG},
		{'D', "deep",   "Keep searching for dependencies even if package already installed", set_deep, OPT_SHORT | OPT_LONG},
		{0,   "root", "Path to the root of build environment", set_root, OPT_LONG | OPT_ARG},
		{'h', "help",    "Print the help message and exit", print_help_wrapper, OPT_SHORT | OPT_LONG},
		{'k', "usepkg",  "Use a package from the binhost", usepkg, OPT_SHORT | OPT_LONG},
		{'v', "verbose",  "be verbose when printing dependencies", verbose, OPT_SHORT | OPT_LONG},
		{0,   "installroot", "Path to the root of install environment", set_installroot, OPT_LONG | OPT_ARG},
		{0,   "target", "Target architecture", set_arch, OPT_LONG | OPT_ARG},
		{0, NULL, NULL, NULL, (opt_opts_t) 0}
};

void print_help_wrapper(Opt* op, char* arg) {
	print_help(opt_handlers);
	exit(0);
}

int main (int argc, char** argv) {
	Emerge* __emerge_main = emerge_new();
	emerge_main = __emerge_main;
	
	emerge_main->atoms = opt_handle(opt_handlers, argc, argv + 1);
	emerge_main->repos = emerge_repos_conf(__emerge_main);
	
	if (!emerge_main->default_repo) {
		errno = 0;
		plog_error("Could not initialize repostiories");
		return 1;
	}
	
	OpenSSL_add_all_digests();
	
	PortageDB* portdb = portagedb_read(__emerge_main);
	
	int out = emerge(__emerge_main);
	
	portagedb_free(portdb);
	emerge_free(__emerge_main);
	
	return out;
}