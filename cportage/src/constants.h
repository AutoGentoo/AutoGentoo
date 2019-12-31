//
// Created by atuser on 4/23/19.
//

#ifndef AUTOGENTOO_CONSTANTS_H
#define AUTOGENTOO_CONSTANTS_H

#define FILEDIR "${PORTDIR}/${CATEGORY}/${PN}/files"

#define WORKDIR "${PORTAGE_BUILDDIR}/work"
#define T "${PORTAGE_BUILDDIR}/temp"
#define D "${PORTAGE_BUILDDIR}/image"
#define HOME "${PORTAGE_BUILDDIR}/homedir"
#define DEFAULT_ROOT "/"
#define DEFAULT_DISTDIR "/usr/portage/distfiles/"

#ifndef PORTAGE_BIN_PATH
#define PORTAGE_BIN_PATH "/usr/lib/cportage/bin"
#endif

#ifndef PORTAGE_BUILDDIR
#define PORTAGE_BUILDDIR "/var/tmp/portage"
#endif

#ifndef PORTAGE_CACHE_OFFSET
#define PORTAGE_CACHE_OFFSET "metadata/cportage-cache"
#endif

#ifndef PORTAGE_TMP_DIR
#define PORTAGE_TMP_DIR ""
#endif

typedef unsigned char* sha_hash;
typedef struct __Package Package;
typedef struct __Ebuild Ebuild;
typedef struct __RequiredUse RequiredUse;
typedef struct __UseFlag UseFlag;
typedef struct __PackageUse PackageUse;
typedef struct __Repository Repository;
typedef struct __Emerge Emerge;
typedef struct __ManifestEntry ManifestEntry;
typedef struct __ManifestHash ManifestHash;
typedef struct __Manifest Manifest;
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
