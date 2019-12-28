//
// Created by atuser on 12/28/19.
//

#ifndef AUTOGENTOO_RESOLVEREQUEST_H
#define AUTOGENTOO_RESOLVEREQUEST_H


#include "../../hacksawpp/Vector.h"
#include "../../hacksawpp/Set.h"
#include "constants.h"
#include "package.h"
#include "ResolvedEbuild.h"

class ResolveRequest {
private:
	Emerge* environ;
	_Vector<ResolvedEbuild*>* ebuilds;
	
	_Vector<ResolveRequest*>* bdepend;
	_Vector<ResolveRequest*>* depend;
	_Vector<ResolveRequest*>* rdepend;
	_Vector<ResolveRequest*>* pdepend;
	
	P_Atom* selected_by;
	ResolveRequest* parent;
	
	void clear_dependencies();
public:
	ResolveRequest (Emerge* environ, ResolveRequest* parent, P_Atom* selector);
	
	ResolvedEbuild* next();
	ResolvedEbuild* current();
	
	~ResolveRequest();
};

#endif //AUTOGENTOO_RESOLVEREQUEST_H
