//
// Created by atuser on 5/5/19.
//

#ifndef AUTOGENTOO_ATOM_H
#define AUTOGENTOO_ATOM_H

typedef struct __P_Atom P_Atom;
typedef struct __AtomVersion AtomVersion;
typedef struct __AtomFlag AtomFlag;
typedef struct __Dependency Dependency;
typedef struct __DependencyTree DependencyTree;

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

typedef enum {
	ATOM_VERSION_NONE,
	ATOM_VERSION_E = 0x1,
	ATOM_VERSION_L = 0x2, //!< <app-misc/foo-1.23	A version strictly before 1.23 is required.
	ATOM_VERSION_G = 0x4, //!< >app-misc/foo-1.23	A version strictly later than 1.23 is required.
	ATOM_VERSION_REV = 0x8,  //!< ~app-misc/foo-1.23	Version 1.23 (or any 1.23-r*) is required.
	
	ATOM_VERSION_ALL = ATOM_VERSION_E | ATOM_VERSION_L | ATOM_VERSION_G,//!< app-misc/foo-1.23		Any version
	ATOM_VERSION_GE = ATOM_VERSION_E | ATOM_VERSION_G, //!< >=app-misc/foo-1.23	Version 1.23 or later is required.
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

struct __AtomFlag {
	char* name;
	atom_use_t option;
	atom_use_default def;
	AtomFlag* next;
};

/**
 * Selects a range of packages (or blocks)
 */
struct __P_Atom {
	char* category;
	char* name;
	char* repository; //!< Default is gentoo
	
	char* key;
	
	char* slot;
	char* sub_slot;
	atom_slot_t sub_opts;
	
	atom_version_t range;
	atom_block_t blocks;
	
	AtomVersion* version;
	int revision;
	
	AtomFlag* useflags;
	
	P_Atom* next; // Only use if this a node
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


P_Atom* atom_new(char* input);
AtomVersion* atom_version_new(char* version_str);
void atomversion_free(AtomVersion* parent);
void atom_free(P_Atom* ptr);
void atomflag_free(AtomFlag* parent);

AtomFlag* atomflag_build(char* name);

int atom_version_compare(AtomVersion* first, AtomVersion* second);

#endif //AUTOGENTOO_ATOM_H
