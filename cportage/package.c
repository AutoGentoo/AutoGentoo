//
// Created by atuser on 4/23/19.
//

#include "package.h"
#include <string.h>
#include <stdlib.h>

atom_cmp_t package_atom_compare(char* atom1, char* atom2) {
	atom_cmp_t out = ATOM_CMP_EQUAL;
	
	atom1 = strdup(atom1);
	atom2 = strdup(atom2);
	
	char* category1 = atom1;
	char* category2 = atom2;
	
	char* package_name1;
	char* package_name2;
	
	char* version_str1 = NULL;
	char* version_str2 = NULL;
	
	char* cat1_splt = strchr(atom1, '/');
	char* cat2_splt = strchr(atom2, '/');
	
	*cat1_splt = 0;
	*cat2_splt = 0;
	
	package_name1 = cat1_splt + 1;
	package_name2 = cat2_splt + 1;
	
	char* ver1_splt = strrchr(package_name1, '-');
	char* ver2_splt = strrchr(package_name2, '-');
	
	if (ver1_splt[1] >= '0' && ver2_splt[1] <= '9') { // Selector has version
		*ver1_splt = 0;
		*ver2_splt = 0;
		
		version_str1 = ver1_splt + 1;
		version_str2 = ver2_splt + 1;
	}
	
	if (strcmp(category1, category2) != 0) {
		out = ATOM_CMP_NO_MATCH;
		goto ret;
	}
	
	if (strcmp(category1, category2) != 0) {
		out = ATOM_CMP_NO_MATCH;
		goto ret;
	}
	
	if (!version_str1) {
		out = ATOM_CMP_EQUAL;
		goto ret;
	}
	
	
	
ret:
	free(atom1);
	free(atom2);
	
	return out;
}