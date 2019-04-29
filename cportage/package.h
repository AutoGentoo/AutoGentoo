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
	NO_DEPEND,
	HAS_DEPEND,
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
	char* target;
	use_select_t selector;
	depend_t depends;
	
	// Selector list
	Dependency* selectors;
	
	// Next target
	Dependency* next;
};

atom_cmp_t package_atom_compare(char* atom1, char* atom2);

#endif //AUTOGENTOO_PACKAGE_H
