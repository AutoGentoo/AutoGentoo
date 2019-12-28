//
// Created by atuser on 12/28/19.
//

#include "ResolveRequest.h"
#include "resolve.h"

ResolveRequest::ResolveRequest(Emerge* environ, ResolveRequest* parent, P_Atom* selector) {
	this->environ = environ;
	this->selected_by = selector;
	this->parent = parent;
	
	this->bdepend = new _Vector<ResolveRequest*>(false);
	this->depend = new _Vector<ResolveRequest*>(false);
	this->rdepend = new _Vector<ResolveRequest*>(false);
	this->pdepend = new _Vector<ResolveRequest*>(false);
	
	this->ebuilds = new _Vector<ResolvedEbuild*>();
	_Vector<ResolvedEbuild*>* priority_two = new _Vector<ResolvedEbuild*>();
	_Vector<ResolvedEbuild*>* priority_three = new _Vector<ResolvedEbuild*>();
	
	Package* pkg = package_resolve_atom(environ, selector);
	
	for (Ebuild* current = pkg->ebuilds; current; current = current->older) {
		package_metadata_init(current);
		if (!ebuild_match_atom(current, selector))
			continue;
		
		keyword_t accept_keyword = KEYWORD_STABLE;
		
		/* Apply package.accept_keywords */
		for (Keyword* keyword = pkg->keywords; keyword; keyword = keyword->next) {
			if (ebuild_match_atom(current, keyword->atom))
				if (keyword->keywords[environ->target_arch] < accept_keyword)
					accept_keyword = keyword->keywords[environ->target_arch];
		}
		
		if (current->keywords[environ->target_arch] == KEYWORD_NONE
		    || current->keywords[environ->target_arch] == KEYWORD_BROKEN)
			continue;
		
		ResolvedEbuild* add_ebuild = new ResolvedEbuild(current, selector);
		
		if (current->keywords[environ->target_arch] >= accept_keyword)
			this->ebuilds->add(add_ebuild);
		else if (current->keywords[environ->target_arch] == KEYWORD_UNSTABLE)
			priority_two->add(add_ebuild);
		else
			priority_three->add(add_ebuild);
	}
	
	this->ebuilds->extend(priority_two);
	this->ebuilds->extend(priority_three);
	
	delete priority_two;
	delete priority_three;
}

void ResolveRequest::clear_dependencies() {
	this->bdepend->foreach([](ResolveRequest* p){delete p;});
	this->depend->foreach([](ResolveRequest* p){delete p;});
	this->rdepend->foreach([](ResolveRequest* p){delete p;});
	this->pdepend->foreach([](ResolveRequest* p){delete p;});
	
	this->bdepend->reset(0);
	this->depend->reset(0);
	this->rdepend->reset(0);
	this->pdepend->reset(0);
}

ResolveRequest::~ResolveRequest() {
	this->clear_dependencies();
}
