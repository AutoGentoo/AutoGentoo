//
// Created by atuser on 4/23/19.
//

#ifndef AUTOGENTOO_CONSTANTS_H
#define AUTOGENTOO_CONSTANTS_H

#define FILEDIR "${PORTDIR}/${CATEGORY}/${PN}/FILES"
#define WORKDIR "${PORTAGE_BUILDDIR}/WORK"
#define T "${PORTAGE_BUILDDIR}/TEMP"
#define D "${PORTAGE_BUILDDIR}/IMAGE"
#define HOME "${PORTAGE_BUILDDIR}/HOMEDIR"
#define DEFAULT_ROOT "/"
#define DEFAULT_DISTDIR "/usr/portage/distfiles/"

typedef unsigned char* sha_hash;
typedef struct __Package Package;
typedef struct __Ebuild Ebuild;
typedef struct __RequiredUse RequiredUse;
typedef struct __UseFlag UseFlag;
typedef struct __PackageUse PackageUse;
typedef struct __Repository Repository;
typedef struct __Emerge Emerge;
typedef struct __Manifest Manifest;
typedef struct __ManifestHash ManifestHash;
typedef struct __InstalledEbuild InstalledEbuild;
typedef struct __InstalledPackage InstalledPackage;
typedef struct __SelectedEbuild SelectedEbuild;
typedef struct __RebuildEbuild RebuildEbuild;
typedef struct __PortageDB PortageDB;
typedef struct __Suggestion Suggestion;
typedef struct __Keyword Keyword;
typedef struct __PortageSet PortageSet;
typedef struct __Backtrack Backtrack;
typedef struct __InstalledBacktrack InstalledBacktrack;
typedef struct __Selected Selected;

/* From atom.h */
typedef struct __P_Atom P_Atom;
typedef struct __AtomVersion AtomVersion;
typedef struct __AtomFlag AtomFlag;
typedef struct __Dependency Dependency;

#endif //AUTOGENTOO_CONSTANTS_H
