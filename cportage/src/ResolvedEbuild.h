//
// Created by atuser on 12/28/19.
//

#ifndef AUTOGENTOO_RESOLVEDEBUILD_H
#define AUTOGENTOO_RESOLVEDEBUILD_H

#include "constants.h"

class ResolvedEbuild {
private:
	Ebuild* ebuild;
	P_Atom* selected_by;

public:
	ResolvedEbuild(Ebuild* parent, P_Atom* selected_by) {
		this->ebuild = parent;
		this->selected_by = selected_by;
	};
	
	Ebuild* get_ebuild() {return this->ebuild;}
};

#endif //AUTOGENTOO_RESOLVEDEBUILD_H
