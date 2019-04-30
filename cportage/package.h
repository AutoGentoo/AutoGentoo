//
// Created by atuser on 4/23/19.
//

#ifndef AUTOGENTOO_PACKAGE_H
#define AUTOGENTOO_PACKAGE_H

#include <autogentoo/hacksaw/tools/vector.h>
#include <stdint.h>
#include <autogentoo/hacksaw/tools/string_vector.h>

typedef struct __Package Package;
typedef struct __Ebuild Ebuild;
typedef struct __Dependency Dependency;
typedef struct __P_Atom P_Atom;
typedef struct __AtomNode AtomNode;

struct __Package {
	uint32_t hash;
	
	char* key; // category/name
	char* category;
	char* name;
	
	Vector* ebuilds;
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

struct __Use {
	char* name;
	int enabled;
};

/**
 * Version of a specfic package, this is what is built
 */
struct __Ebuild {
	char* pn;
	char* pv;
	char* pr;
	char* slot;
	char* eapi;
	
	/* Uninitalized until we emerge it */
	char* p;
	char* pvr;
	char* pf;
	char* category;
	
	/* Cached in the database */
	Dependency* depend;
	Dependency* bdepend;
	Dependency* rdepend;
	Dependency* pdepend; //!< Post install dependencies so no circular depends

	Vector* use; //!< Read iuse, then apply globals (make.conf), then package.use
	Vector* feature_restrict;
	keyword_t keywords[ARCH_END];
	
	Dependency* required_use;
	Dependency* src_uri;
};

typedef enum {
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
	ATOM_CMP_EQUAL,
	ATOM_CMP_NO_MATCH, // Not the same package
	ATOM_CMP_LESS,
	ATOM_CMP_GREATER
} atom_cmp_t;

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
	ATOM_VERSION_ALL,//!< app-misc/foo-1.23		Any version
	ATOM_VERSION_GE, //!< >=app-misc/foo-1.23	Version 1.23 or later is required.
	ATOM_VERSION_G,  //!< >app-misc/foo-1.23	A version strictly later than 1.23 is required.
	ATOM_VERSION_A,  //!< ~app-misc/foo-1.23	Version 1.23 (or any 1.23-r*) is required.
	ATOM_VERSION_E,  //!< =app-misc/foo-1.23	Exactly version 1.23 is required. If at all possible, use the ~ form to simplify revision bumps.
	ATOM_VERSION_LE, //!< <=app-misc/foo-1.23	Version 1.23 or older is required.
	ATOM_VERSION_L,  //!< <app-misc/foo-1.23	A version strictly before 1.23 is required.
} atom_version_t;

typedef enum {
	ATOM_BLOCK_NONE,
	ATOM_BLOCK_SOFT,
	ATOM_BLOCK_HARD
} atom_block_t;

typedef enum {
	ATOM_SLOT_ALL = 0,
	ATOM_SLOT_
} atom_slot_t;

struct __AtomNode {
	char* v;
	AtomNode* next;
};

/**
 * Selects a range of packages (or blocks)
 */
struct __P_Atom {
	char* category;
	char* name;
	
	atom_version_t range;
	atom_block_t blocks;
	
	AtomNode* version;
	AtomNode* useflags;
};

atom_cmp_t ebuild_atom_compare(Ebuild* ebuild, P_Atom* atom);
P_Atom* atom_new(char* cat, char* name);
AtomNode* atom_version_new(char* version_str);
void atomnode_free(AtomNode* parent);
void atom_free(P_Atom* ptr);

#endif //AUTOGENTOO_PACKAGE_H
