//
// Created by atuser on 10/31/17.
//


#include <autogentoo/hacksaw/tools/vector.h>

#if !defined(HACKSAW_COMPILER_DEPEND_H) && !defined(HACKSAW_LANGUAGE_H)
#define HACKSAW_COMPILER_DEPEND_H

typedef struct __check_use CheckUse;
typedef struct __expr DependExpression;
typedef struct __atom Atom;
typedef struct __use Use;
typedef struct __atom_opts AtomOpts;
typedef struct __require_use RequireUse;
typedef struct __atom_slot AtomSlot;

typedef enum {
	ALL,
	EQUAL,
	DEP_REVISION,
	LESS_E,
	GREAT_E,
	LESS,
	GREAT,
} atom_t;

typedef enum {
	NO_BLOCK,
	SOFT_BLOCK,
	HARD_BLOCK
} block_t;

typedef enum {
	USE_EXPR,
	SEL_EXPR,
	EXPR_EXPR
} expr_t;

typedef enum {
	YES_USE,
	NO_USE,
	EXACTLY_ONE,
	AT_LEAST_ONE,
	AT_MOST_ONE
} use_t;

typedef enum {
	DISABLED,
	ENABLED,
	SAME, //foo[bar=] bar? ( app-misc/foo[bar] ) !bar? ( app-misc/foo[-bar] )
	OPPOSITE,
	CHECK,
	OPPOSOTE_CHECK,
} ruse_t;

typedef enum {
	DEFAULT_DISABLE,
	DEFAULT_ENABLE,
	DEFAULT_NONE
} druse_t;

typedef enum {
	SLOT_NO_REBUILD,
	SLOT_REBUILD
} slot_t;

struct __check_use {
	Use* to_check;
	DependExpression* inner;
};

struct __atom_opts {
	atom_t status;
	block_t block;
	Vector* required_use;
};

struct __expr {
	expr_t type;
	CheckUse* c_use; // Use if type == USE_EXPR
	Atom* select; // Use if type == SEL_EXPR
	Vector* dependexpressions; // Use if type == EXPR_EXPR
};

struct __use {
	use_t type;
	char* str;
};

struct __require_use {
	char* flag;
	ruse_t status;
	druse_t _default;
};

struct __atom_slot {
	slot_t rebuild;
	char* main_slot;
	char* sub_slot;
};

struct __atom {
	char* atom;
	AtomOpts opts;
	AtomSlot slot;
};

CheckUse* new_check_use(Use* use, DependExpression* inner);

void add_dependexpression(Vector* list, Vector* exp);

DependExpression* new_dependexpression(void* ptr, expr_t type);

Atom* new_atom(char* str, AtomOpts opts);

void set_atom_opts(AtomOpts* opts, atom_t status, block_t block);

Use* new_use(char* str, use_t type);

/* Debug */
void debug_dependexpression(DependExpression* expr);

void print_c_use(CheckUse* c_use);

void print_sel(Atom* selection);

void print_require_use(RequireUse r);

/* Free */
void free_dependexpression(DependExpression* expr);

void free_check_use(CheckUse* ptr);

void free_use(Use* ptr);

void free_selection(Atom* ptr);

#endif //HACKSAW_COMPILER_DEPEND_H