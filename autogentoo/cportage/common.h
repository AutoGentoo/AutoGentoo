//
// Created by tumbar on 12/13/20.
//

#ifndef AUTOGENTOO_COMMON_H
#define AUTOGENTOO_COMMON_H

#include "Python.h"

typedef struct Portage_prv Portage;
typedef struct Ebuild_prv Ebuild;
typedef struct Atom_prv Atom;
typedef struct AtomVersion_prv AtomVersion;
typedef struct AtomFlag_prv AtomFlag;
typedef struct Dependency_prv Dependency;
typedef struct Package_prv Package;
typedef struct RequiredUse_prv RequiredUse;
typedef struct UseFlag_prv UseFlag;

extern PyTypeObject PyAtomFlagType;
extern PyTypeObject PyAtomVersionType;
extern PyTypeObject PyAtomType;

extern PyTypeObject PyUseFlagType;
extern PyTypeObject PyRequiredUseType;

extern PyTypeObject PyDependencyType;
extern PyTypeObject PyPackageType;
extern PyTypeObject PyEbuildType;

extern PyTypeObject PyPortageType;
extern Portage* global_portage;


#endif //AUTOGENTOO_COMMON_H
