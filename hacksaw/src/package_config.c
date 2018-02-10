#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <autogentoo/hacksaw/portage/package_config.h>
#include <autogentoo/hacksaw/portage/directory.h>

PackageMeta* package_meta_new() {
	PackageMeta* out = malloc(sizeof(PackageMeta));
	out->key = NULL;
	out->use = vector_new(sizeof(PackageUse*), REMOVE | UNORDERED);
	out->license = vector_new(sizeof(PackageLicense*), REMOVE | UNORDERED);
	out->mask = vector_new(sizeof(PackageMask*), REMOVE | UNORDERED);
	out->unmask = vector_new(sizeof(PackageUnmask*), REMOVE | UNORDERED);
	
	return out;
}

void package_conf_add(PackageConf* pconf, char* key, void* data, packagemeta_t dest) {
	if (small_map_get((SmallMap*) pconf, key) == NULL) {
		PackageMeta* new_meta_temp = package_meta_new();
		small_map_insert((SmallMap*) pconf, key, &new_meta_temp);
	}
	
	Vector* destvec;
	
	switch (dest) {
		case PM_LICENSE:
			destvec = pconf;
			break;
		case PM_USE:
			break;
		case PM_MASK:
			break;
		case PM_UNMASK:
			break;
	}
}

void read_package_use(Portage* portage, PackageConf* dest) {
	PortageDirectory* pdir = portage_directory_read(portage, "package.use");
	
	char* line;
	size_t len = 0;
	
	int i;
	for (i = 0; i != pdir->fp_list->n; i++) {
		FILE* fp = *(FILE**) vector_get(pdir->fp_list, i);
		
		len = 0;
		line = NULL;
		
		while (getline(&line, &len, fp) != -1) {
			line[strlen(line) - 1] = 0; // Remove the newline
			
		}
	}
	
	portage_directory_free(pdir);
}

void read_package_license(Portage* portage, PackageConf* dest);

void read_package_mask(Portage* portage, PackageConf* dest);

void read_package_unmask(Portage* portage, PackageConf* dest);