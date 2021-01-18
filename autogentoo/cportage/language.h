#ifndef CPORTAGE_COMPILER_SHARE_H
#define CPORTAGE_COMPILER_SHARE_H

#include "dependency.h"
#include "use.h"

typedef void DependencyBuffers;
typedef void RequiredUseBuffers;

Atom* atom_parse(DependencyBuffers* buffers, const char* input);
Dependency* depend_parse(DependencyBuffers* buffers, const char* input);
RequiredUse* required_use_parse(RequiredUseBuffers* buffers, const char* input);
DependencyBuffers* depend_allocate_buffers();
RequiredUseBuffers* required_use_allocate_buffers();
void depend_free_buffers(DependencyBuffers*);
void required_use_free_buffers(RequiredUseBuffers*);
uint32_t depend_init();
uint32_t required_use_init();
void depend_free();
void required_use_free();

#endif // HACKSAW_COMPILER_SHARE_H
