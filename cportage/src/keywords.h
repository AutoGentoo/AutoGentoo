//
// Created by atuser on 5/5/19.
//

#ifndef AUTOGENTOO_KEYWORDS_H
#define AUTOGENTOO_KEYWORDS_H

#include <stdio.h>
#include "cportage_defines.h"
#include "atom.h"

struct __Keyword {
    P_Atom* atom;
    keyword_t keywords[ARCH_END];
    Keyword* next;
};

static struct __keyword_link_strct {
    arch_t l;
    char* str;
} keyword_links[] = {
        {ARCH_AMD64, "amd64"},
        {ARCH_X86,   "x86"},
        {ARCH_ARM,   "arm"},
        {ARCH_ARM64, "arm64"},
        {ARCH_HPPA,  "hppa"},
        {ARCH_IA64,  "ia64"},
        {ARCH_PPC,   "ppc"},
        {ARCH_PPC64, "ppc64"},
        {ARCH_SPARC, "sparc"},
        {ARCH_END,   ""}
};

#include "emerge.h"

arch_t get_arch(char* search);

void keyword_parse(keyword_t* out, char* line);

void accept_keyword_parse(FILE* fp, Vector* keywords);

void keyword_free(Keyword* keyword);

void emerge_parse_keywords(Emerge* emerge);

#endif //AUTOGENTOO_KEYWORDS_H
