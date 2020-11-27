//
// Created by atuser on 6/23/19.
//

#ifndef AUTOGENTOO_HOST_ENVIRONMENT_H
#define AUTOGENTOO_HOST_ENVIRONMENT_H

typedef struct __HostEnvironment HostEnvironment;
typedef struct __PortageEntry PortageEntry;

#include "host.h"


struct __HostEnvironment {
    Host* parent;

    char* cflags; //!< The gcc passed to C programs, try -march=native :)
    char* cxxflags; //!< The gcc passed only to CXX programs
    char* use; //!< use flags

    char* portage_tmpdir; //!< build dir
    char* portdir; //!< ebuild portage tree
    char* distdir; //!< distfiles
    char* pkgdir; //!< path to binaries
    char* portage_logdir; //!< logs

    char* lc_messages; //!< Log message language

    SmallMap* extra; //!< A list of extra entries to go into make.conf

    Vector* package_use;
    Vector* package_accept_keywords;
    StringVector* package_mask;
    Vector* package_env;

    char* make_conf_sha;
    char* package_use_sha;
    char* package_accept_keywords_sha;
    char* package_mask_sha;
    char* package_env_sha;
};

struct __PortageEntry {
    char* atom;

    int flags_n;
    char** flags;
};

HostEnvironment* host_environment_new(Host* parent);

int host_environment_write(Host* target);

void host_environment_backup(Host* target);

void host_environment_write_make_conf(Host* target);

void host_environment_free(HostEnvironment* ptr);


/**
 * MAY NOT NEED
 * Checks if host_path/etc/portage/[filename] is a directory and shrinks it to one file
 * @param target host to look in
 * @param filename [package.use, package.accept_keywords] etc.
 * @return 0: no action needed, 1: error, 2: ignore hash (regen needed)
 */
//int host_environment_convert_portagedir(Host* target, char* filename);

#endif //AUTOGENTOO_HOST_ENVIRONMENT_H
