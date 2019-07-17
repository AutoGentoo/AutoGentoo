//
// Created by atuser on 6/23/19.
//

#ifndef AUTOGENTOO_HOST_ENVIRONMENT_H
#define AUTOGENTOO_HOST_ENVIRONMENT_H

typedef struct __HostEnvironment HostEnvironment;
typedef struct __PortageEntry PortageEntry;

typedef enum {
	ACCEPT_KEYWORD_STABLE,
	ACCEPT_KEYWORD_UNSTABLE,
	
	USE_ENABLE,
	USE_DISABLE
} portage_env_t;

typedef enum {
	HOST_ENVIRON_CFLAGS,
	HOST_ENVIRON_CXXCFLAGS,
	HOST_ENVIRON_USE
} host_environ_t;

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
};

struct __PortageEntry {
	char** comments;
	int n;
	
	char* atom;
	portage_env_t status;
};

#endif //AUTOGENTOO_HOST_ENVIRONMENT_H
