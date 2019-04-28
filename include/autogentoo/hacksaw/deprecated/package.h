//
// Created by atuser on 10/18/17.
//


#include "manifest.h"

#ifdef AUTOGENTOO_PACKAGE_H
#else
#define AUTOGENTOO_PACKAGE_H

#include "repository.h"

typedef struct __Package Package;
typedef struct __Ebuild Ebuild;
typedef struct __EbuildVersion EbuildVersion;

/**
 * Package - The main wrapper for a set of ebuilds
 * The manifest
 */
struct __Package {
	Repository* repo;
	Manifest* manifest;
	Vector* ebuilds; // Vector<EbuildVersion>, just create version not the entire ebuild
	
	char* category;
	char* name;
};


struct __EbuildVersion {
	Vector* version;
	char* suffix;
	unsigned char revision; // 0 to disable
};

struct __Ebuild {
	Package* parent;
	Conf* metadata;
	EbuildVersion* version; // Pointer to parent->ebuilds
};

void category_read (Repository* repo, char* name);
Package* package_new (Repository* repo, char* category, char* name);
void package_get_ebuilds (Package* pkg);
void package_get_file (Package* pkg, char* filename);


#endif //HACKSAW_PACKAGE_H
