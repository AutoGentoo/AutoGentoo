//
// Created by atuser on 12/28/19.
//

#ifndef AUTOGENTOO_RESOLVEDPACKAGE_H
#define AUTOGENTOO_RESOLVEDPACKAGE_H

#include <cstring>
#include "ResolveRequest.h"

class ResolvedPackage {
private:
	Emerge* environ;
	
	char* key;
	
	_Vector<ResolveRequest*>* resolved;
	
	static int compare_ebuilds(ResolvedEbuild* e1, ResolvedEbuild* e2) {return strcmp(e1->get_ebuild()->slot, e2->get_ebuild()->slot) != 0;}
	_Set<ResolvedEbuild*, compare_ebuilds>* selected;
public:
	ResolvedPackage(Emerge* environ, char* key);
	
	_Set<ResolvedEbuild*, compare_ebuilds>* get_selected();
};


#endif //AUTOGENTOO_RESOLVEDPACKAGE_H
