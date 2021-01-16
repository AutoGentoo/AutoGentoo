#ifndef CPORTAGE_COMPILER_SHARE_H
#define CPORTAGE_COMPILER_SHARE_H

#include "dependency.h"
#include "use.h"

Atom* atom_parse(void* buffers, const char* input);
Dependency* depend_parse(void* buffers, const char* input);
RequiredUse* required_use_parse(void* buffers, const char* input);
void* depend_allocate_buffers();
void* required_use_allocate_buffers();
void depend_free_buffers(void*);
void required_use_free_buffers(void*);
uint32_t depend_init();
uint32_t required_use_init();
void depend_free();
void required_use_free();

#endif // HACKSAW_COMPILER_SHARE_H
