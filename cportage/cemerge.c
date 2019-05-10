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
#include <string.h>


static Repository* repo_main;
static Emerge* emerge_main;

void set_quiet(Opt* opt, char* arg) {
	emerge_main->options |= EMERGE_QUIET;
}

void set_deep(Opt* opt, char* arg) {
	emerge_main->options |= EMERGE_DEEP;
}

void set_buildroot(Opt* opt, char* arg) {
	emerge_main->options |= EMERGE_BUILDROOT;
	if (emerge_main->buildroot)
		free(emerge_main->buildroot);
	emerge_main->buildroot = strdup(arg);
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

void set_repodir(Opt* opt, char* args) {
	emerge_main->repo->
}

void print_help_wrapper(Opt* op, char* arg);

Opt opt_handlers[] = {
		{'q', "quiet",  "Don't print the build logs during emerge", set_quiet, OPT_SHORT | OPT_LONG},
		{'D', "deep",   "Keep searching for dependencies even if package already installed", set_deep, OPT_SHORT | OPT_LONG},
		{0,   "buildroot", "Path to the root of build environment", set_buildroot, OPT_LONG | OPT_ARG},
		{'h', "help",    "Print the help message and exit", print_help_wrapper, OPT_SHORT | OPT_LONG},
		{0,   "installroot", "Path to the root of install environment", set_installroot, OPT_LONG | OPT_ARG},
		{0,   "target", "Target architecture", set_arch, OPT_LONG | OPT_ARG},
		{0,   "repodir", "Directory to metadata of repository", set_arch, OPT_LONG | OPT_ARG},
		{0, NULL, NULL, NULL, (opt_opts_t) 0}
};

void print_help_wrapper(Opt* op, char* arg) {
	print_help(opt_handlers);
	exit(0);
}

int main (int argc, char** argv) {
	emerge_main = emerge_new();
	emerge_main->repo = repository_new("gentoo", emerge_main->root, "etc/portage");
	
	char** emerge_atoms = opt_handle(opt_handlers, argc, argv + 1);
	
	OpenSSL_add_all_digests();
	sha_hash test_hash;
	int md_len;
	
	PLOG_BENCHMARK(md_len = portage_get_hash(&test_hash, "/etc/portage/make.conf", EVP_sha256());, "PORTAGE_GET_HASH")
	
	Manifest* category_manifest;
	PLOG_BENCHMARK({
		category_manifest = manifest_metadata_parse("test");
	}, "CATEGORY_MANIFEST")
	
	PLOG_BENCHMARK({
		manifest_metadata_deep(category_manifest);
	}, "ALL_MANIFEST")
	
	
	Manifest* current_cat;
	Manifest* current_pkg;
	
	PLOG_BENCHMARK({
		for (current_cat = category_manifest; current_cat; current_cat = current_cat->next) {
			for(current_pkg = current_cat->parsed; current_pkg; current_pkg = current_pkg->next) {
				package_init(emerge_main->repo, current_cat, current_pkg);
			}
		}
	}, "PKG_INIT")
	
	Package* gcc = (Package*)map_get(emerge_main->repo->packages, "sys-devel/gcc");
	repository_parse_keywords(emerge_main->repo);
	
	Ebuild* gcc_resolve = atom_resolve_ebuild(emerge_main->repo, atom_parse("<=sys-devel/gcc-8.3.0"), emerge_main->target_arch);
	printf("%s-r%d\n", gcc_resolve->version->full_version, gcc_resolve->revision);
	return 0;
}