//
// Created by atuser on 5/9/19.
//

#ifndef AUTOGENTOO_CPORTAGE_DEFINES_H
#define AUTOGENTOO_CPORTAGE_DEFINES_H

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
	KEYWORD_BROKEN,
	KEYWORD_NONE,
	KEYWORD_UNSTABLE,
	KEYWORD_STABLE,
} keyword_t;

#endif //AUTOGENTOO_CPORTAGE_DEFINES_H
