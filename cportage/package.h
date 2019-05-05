//
// Created by atuser on 4/23/19.
//

#ifndef AUTOGENTOO_PACKAGE_H
#define AUTOGENTOO_PACKAGE_H

#include <autogentoo/hacksaw/vector.h>
#include <stdint.h>
#include <autogentoo/hacksaw/string_vector.h>
#include "constants.h"

typedef struct __Package Package;
typedef struct __Ebuild Ebuild;
typedef struct __Dependency Dependency;
typedef struct __P_Atom P_Atom;
typedef struct __AtomVersion AtomVersion;
typedef struct __AtomFlag AtomFlag;

struct __Package {
	uint32_t hash;
	
	char* key; // category/name
	char* category;
	char* name;
	
	Ebuild* ebuilds;
};

typedef enum {
	KEYWORD_NONE,
	KEYWORD_UNSTABLE,
	KEYWORD_STABLE
} keyword_t;

/**
 * Only includes supported architectures
 * amd64	x86	alpha	arm	arm64	hppa	ia64	ppc	ppc64	sparc
 */
typedef enum {
	ARCH_AMD64,
	ARCH_X86,
	ARCH_ARM,
	ARCH_ARM64,
	ARCH_HPPA,
	ARCH_IA64,
	ARCH_PPC,
	ARCH_PPC64,
	ARCH_SPARC,
	ARCH_END
} arch_t;

typedef enum {
	USE_NONE,
	USE_DISABLE, //!< !
	USE_ENABLE, //!< ?
	USE_LEAST_ONE, //!< ||
	USE_EXACT_ONE, //!< ^^
	USE_MOST_ONE, //!< ??
} use_select_t;

typedef enum {
	IS_ATOM,
	HAS_DEPENDS,
} depend_t;

typedef enum {
	ATOM_USE_DISABLE, //!< atom[-bar]
	ATOM_USE_ENABLE, //!< atom[bar]
	ATOM_USE_ENABLE_IF_ON, //!< atom[bar?]
	ATOM_USE_DISABLE_IF_OFF, //!< atom[!bar?]
	ATOM_USE_EQUAL, //!< atom[bar=]
	ATOM_USE_OPPOSITE //!< atom[!bar=]
} atom_use_t;

typedef enum {
	ATOM_NO_DEFAULT, //!< use
	ATOM_DEFAULT_ON, //!< use(+)
	ATOM_DEFAULT_OFF, //!< use(-)
} atom_use_default;


#include "use.h"

/**
 * Version of a specfic package, this is what is built
 */
struct __Ebuild {
	char* category;
	char* pn;
	char* pv;
	char* pr;
	char* slot;
	char* eapi;
	
	/* Uninitalized until we emerge it */
	char* p;
	char* pvr;
	char* pf;
	
	/* Cached in the database */
	Dependency* depend;
	Dependency* bdepend;
	Dependency* rdepend;
	Dependency* pdepend; //!< Post install dependencies so no circular depends
	
	Vector* use; //!< Read iuse, then apply globals (make.conf), then package.use
	Vector* feature_restrict;
	keyword_t keywords[ARCH_END];
	
	RequiredUse* required_use;
	Dependency* src_uri;
	
	AtomVersion* version;
	sha_hash meta_sha512;
	
	Ebuild* older;
	Ebuild* newer;
};

struct __AtomFlag {
	char* name;
	atom_use_t option;
	atom_use_default def;
	AtomFlag* next;
};

/**
 * target? ( selector selector selector ) next_target? ( ... ) depend
 */
struct __Dependency {
	P_Atom* atom; // NULL if it has depends
	char* target; // NULL if has atom
	
	use_select_t selector;
	depend_t depends;
	
	// Selector list
	Dependency* selectors;
	
	// Next target
	Dependency* next;
};

typedef enum {
	ATOM_VERSION_NONE,
	ATOM_VERSION_E = 0x1,
	ATOM_VERSION_L = 0x2, //!< <app-misc/foo-1.23	A version strictly before 1.23 is required.
	ATOM_VERSION_G = 0x4, //!< >app-misc/foo-1.23	A version strictly later than 1.23 is required.
	
	ATOM_VERSION_ALL = ATOM_VERSION_E | ATOM_VERSION_L | ATOM_VERSION_G,//!< app-misc/foo-1.23		Any version
	ATOM_VERSION_GE = ATOM_VERSION_E | ATOM_VERSION_G, //!< >=app-misc/foo-1.23	Version 1.23 or later is required.
	ATOM_VERSION_REV,  //!< ~app-misc/foo-1.23	Version 1.23 (or any 1.23-r*) is required.
	ATOM_VERSION_LE = ATOM_VERSION_L | ATOM_VERSION_E, //!< <=app-misc/foo-1.23	Version 1.23 or older is required.
} atom_version_t;

typedef enum {
	ATOM_BLOCK_NONE,
	ATOM_BLOCK_SOFT,
	ATOM_BLOCK_HARD
} atom_block_t;

typedef enum {
	ATOM_SLOT_IGNORE,
	ATOM_SLOT_REBUILD
} atom_slot_t;

typedef enum {
	ATOM_PREFIX_ALPHA,
	ATOM_PREFIX_BETA,
	ATOM_PREFIX_PRE,
	ATOM_PREFIX_RC,
	ATOM_PREFIX_NONE,
	ATOM_PREFIX_P,
	ATOM_REVISION
} atom_version_pre_t;

struct __AtomVersion {
	char* full_version; //!< Only set on the first one
	char* v; //!< If there is a prefix, only include the integer part, if none 0
	atom_version_pre_t prefix;
	AtomVersion* next;
};

/**
 * Selects a range of packages (or blocks)
 */
struct __P_Atom {
	char* category;
	char* name;
	
	char* slot;
	char* sub_slot;
	atom_slot_t sub_opts;
	
	atom_version_t range;
	atom_block_t blocks;
	
	AtomVersion* version;
	AtomFlag* useflags;
};

#include "database.h"
#include "manifest.h"

P_Atom* atom_new(char* input);
AtomVersion* atom_version_new(char* version_str);
void atomversion_free(AtomVersion* parent);
void atom_free(P_Atom* ptr);
void atomflag_free(AtomFlag* parent);

Dependency* dependency_build_atom(P_Atom* atom);
Dependency* dependency_build_use(char* use_flag, use_select_t type, Dependency* selector);

AtomFlag* atomflag_build(char* name);

int atom_version_compare(AtomVersion* first, AtomVersion* second);

void package_metadata_init(Ebuild* ebuild, Manifest* atom_man);
Ebuild* package_init(Repository* repo, Manifest* category_man, Manifest* atom_man);

#endif //AUTOGENTOO_PACKAGE_H
