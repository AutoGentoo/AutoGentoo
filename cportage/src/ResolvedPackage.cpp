//
// Created by atuser on 12/28/19.
//

#include "ResolvedPackage.h"
#include <vector>

_Set<Ebuild*, ResolvedPackage::compare_ebuilds>* ResolvedPackage::get_selected() {
	return this->selected;
}

ResolvedPackage::ResolvedPackage(Emerge* environ, char* key) {
	this->environ = environ;
	this->key = strdup(key);
	
	this->resolved = new _Vector<ResolveRequest*>();
	this->selected = new _Set<Ebuild*,ResolvedPackage::compare_ebuilds>();
}
