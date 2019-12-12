//
// Created by atuser on 12/6/19.
//

#include <stdlib.h>
#include <errno.h>
#include "resolve.h"
#include "package.h"
#include "database.h"
#include <string.h>

int pd_compare_range(int cmp, atom_version_t range) {
	if ((cmp == 0 && range & ATOM_VERSION_E)
		|| (cmp > 0 && range & ATOM_VERSION_G)
		|| (cmp < 0 && range & ATOM_VERSION_L))
		return 1;
	return 0;
}

int ebuild_match_atom(Ebuild* ebuild, P_Atom* atom) {
	if (strcmp(ebuild->parent->key, atom->key) !=0)
		return 0;
	
	int slot_cmp = 0;
	int sub_slot_cmp = 0;
	
	if (atom->slot) {
		slot_cmp = strcmp(ebuild->slot, atom->slot);
		if (ebuild->sub_slot && atom->sub_slot)
			sub_slot_cmp = strcmp(ebuild->sub_slot, atom->sub_slot);
		
		if (atom->range == ATOM_VERSION_ALL && atom->version == NULL)
			return !slot_cmp;
		
		/* Only compare the slots */
		if (!pd_compare_range(slot_cmp, atom->range))
			return 0;
		if (!pd_compare_range(sub_slot_cmp, atom->range))
			return 0;
	}
	
	int cmp = 0;
	int cmp_rev = 0;
	if (atom->version) {
		cmp = atom_version_compare(ebuild->version, atom->version);
		cmp_rev = ebuild->revision - atom->revision;
	} else {
		return 1;
	}
	
	if (cmp == 0 && atom->range & ATOM_VERSION_E) {
		if (atom->range == ATOM_VERSION_E) {
			return cmp_rev == 0;
		} else if (atom->range == ATOM_VERSION_REV)
			return 1;
	}
	
	if (pd_compare_range(cmp, atom->range))
		return 1;
	
	return 0;
}

ResolvedEbuild* resolved_ebuild_new(Ebuild* ebuild, P_Atom* atom) {
	ResolvedEbuild* temp = malloc(sizeof(ResolvedEbuild));
	temp->next = NULL;
	temp->back = NULL;
	temp->installed = portagedb_resolve_installed(ebuild->parent->parent->parent->database, atom, ebuild->slot);
	
	temp->useflags = NULL;
	temp->ebuild = ebuild;
	temp->explicit_flags = NULL;
	
	temp->action = PORTAGE_NEW;
	temp->unstable_keywords = 0;
	
	if (temp->installed)
		temp->action = ebuild_installedebuild_cmp(temp->ebuild, temp->installed);
	
	return temp;
}

Package* package_resolve_atom(Emerge* emerge, P_Atom* atom) {
	Repository* repo = NULL;
	for (Repository* current = emerge->repos; current; current = current->next) {
		if (strcmp(atom->repository, current->name) == 0) {
			repo = current;
			break;
		}
	}
	
	if (!repo) {
		errno = EINVAL;
		plog_error("Repository not found %s", atom->repository);
		return NULL;
	}
	
	Package* target_pkg = map_get(repo->packages, atom->key);
	
	return target_pkg;
}

ResolvedPackage* resolved_ebuild_resolve(Emerge* em, P_Atom* atom) {
	Package* pkg = package_resolve_atom(em, atom);
	if (!pkg) {
		return NULL;
	}
	
	ResolvedPackage* pkg_out = malloc(sizeof(ResolvedPackage));
	pkg_out->environ = em;
	pkg_out->pre_dependency = vector_new(VECTOR_ORDERED | VECTOR_REMOVE);
	pkg_out->post_dependency = vector_new(VECTOR_ORDERED | VECTOR_REMOVE);
	pkg_out->parents = vector_new(VECTOR_ORDERED | VECTOR_REMOVE);
	pkg_out->current = NULL;
	pkg_out->ebuilds = NULL;
	
	ResolvedEbuild* out = NULL;
	ResolvedEbuild* current_link = NULL;
	
	ResolvedEbuild* unstable_start = NULL;
	ResolvedEbuild* unstable_end = NULL;
	
	ResolvedEbuild* none_start = NULL;
	ResolvedEbuild* none_end = NULL;
	
	Ebuild* current;
	for (current = pkg->ebuilds; current; current = current->older) {
		package_metadata_init(current);
		if (ebuild_match_atom(current, atom)) {
			keyword_t accept_keyword = KEYWORD_STABLE;
			
			/* Apply package.accept_keywords */
			for (Keyword* keyword = pkg->keywords; keyword; keyword = keyword->next) {
				if (ebuild_match_atom(current, keyword->atom))
					if (keyword->keywords[pkg->parent->parent->target_arch] < accept_keyword)
						accept_keyword = keyword->keywords[pkg->parent->parent->target_arch];
			}
			
			if (current->keywords[pkg->parent->parent->target_arch] == KEYWORD_NONE
				|| current->keywords[pkg->parent->parent->target_arch] == KEYWORD_BROKEN)
				continue;
			
			ResolvedEbuild* temp = resolved_ebuild_new(current, atom);
			
			ResolvedEbuild** start = NULL;
			ResolvedEbuild** end = NULL;
			
			if (current->keywords[pkg->parent->parent->target_arch] >= accept_keyword) {
				/* Preferably use this set of ebuilds, this is added before
				 * the non-accepted ebuilds
				 * */
				start = &out;
				end = &current_link;
			}
			else if (current->keywords[pkg->parent->parent->target_arch] == KEYWORD_UNSTABLE) {
				temp->unstable_keywords = 1;
				start = &unstable_start;
				end = &unstable_end;
			}
			else {
				temp->unstable_keywords = 2;
				start = &none_start;
				end = &none_end;
			}
			
			if (!(*start)) {
				*start = temp;
				*end = temp;
			}
			else {
				(*end)->next = temp;
				temp->back = *end;
				*end = temp;
			}
		}
	}
	
	if (!unstable_start)
		unstable_start = none_start;
	else {
		unstable_end->next = none_start;
		if (none_start)
			none_start->back = unstable_end;
	}
	
	if (!out)
		out = unstable_start;
	else {
		current_link->next = unstable_start;
		if (unstable_start)
			unstable_start->back = current_link;
	}
	
	pkg_out->current = pkg_out->ebuilds = out;
	
	return pkg_out;
}

void resolved_ebuild_free(ResolvedEbuild* ptr) {
	if (!ptr)
		return;
	
	use_free(ptr->useflags);
	use_free(ptr->explicit_flags);
	resolved_ebuild_free(ptr->next);
	free(ptr);
}

int resolved_ebuild_is_blocked(Emerge* em, ResolvedEbuild* re) {
	return re->ebuild->blocked;
}

int resolved_ebuild_use_build(ResolvedEbuild* parent, ResolvedEbuild* out, P_Atom* atom) {
	struct {
		atom_use_t atom_use;
		use_t resolved_use; /* From parent */
		use_t target_use;
	} atom_use_enum_links[] = {
			{ATOM_USE_DISABLE, -1, USE_DISABLE},
			{ATOM_USE_ENABLE, -1, USE_ENABLE},
			{ATOM_USE_ENABLE_IF_ON, USE_ENABLE, USE_ENABLE},
			{ATOM_USE_DISABLE_IF_OFF, USE_DISABLE, USE_DISABLE},
			{ATOM_USE_EQUAL, USE_DISABLE, USE_DISABLE},
			{ATOM_USE_EQUAL, USE_ENABLE, USE_ENABLE},
			{ATOM_USE_OPPOSITE, USE_DISABLE, USE_ENABLE},
			{ATOM_USE_OPPOSITE, USE_ENABLE, USE_DISABLE},
	};
	
	/* Apply the explicit flags */
	for (AtomFlag* current_use = atom->useflags; current_use; current_use = current_use->next) {
		/*
		ATOM_USE_DISABLE, //!< atom[-bar]
		ATOM_USE_ENABLE, //!< atom[bar]
		ATOM_USE_ENABLE_IF_ON, //!< atom[bar?]
		ATOM_USE_DISABLE_IF_OFF, //!< atom[!bar?]
		ATOM_USE_EQUAL, //!< atom[bar=]
		ATOM_USE_OPPOSITE //!< atom[!bar=]
		
		ATOM_NO_DEFAULT, //!< use
		ATOM_DEFAULT_ON, //!< use(+)
		ATOM_DEFAULT_OFF, //!< use(-)
		
		 */
		UseFlag* parent_flag = NULL;
		if (parent)
			parent_flag = use_get(parent->useflags, current_use->name);
		
		UseFlag* flag = use_get(out->useflags, current_use->name);
		use_t def_status = current_use->def == ATOM_DEFAULT_ON ? USE_ENABLE : USE_DISABLE;
		use_t resolved_status = def_status;
		if (!flag && current_use->def != ATOM_NO_DEFAULT) {
			for (int i = 0; i < sizeof(atom_use_enum_links) / sizeof(atom_use_enum_links[0]); i++) {
				if (current_use->option == atom_use_enum_links[i].atom_use
				    && (atom_use_enum_links[i].resolved_use == -1
				        || ((parent_flag && parent_flag->status == atom_use_enum_links[i].resolved_use)
				             || def_status == atom_use_enum_links[i].resolved_use))
				)
					resolved_status = atom_use_enum_links[i].target_use;
			}
			
			if (def_status != resolved_status) {
				char* atom_str = atom_get_str(atom);
				portage_die("Default use for %s did not match %s", current_use->name, atom_str);
				return 0;
			}
		}
		else if (!flag) /* Invalid flag name, ignore */
			continue;
		else {
			use_t old_status = flag->status;
			
			for (int i = 0; i < sizeof(atom_use_enum_links) / sizeof(atom_use_enum_links[0]); i++) {
				if (current_use->option == atom_use_enum_links[i].atom_use
				    && (atom_use_enum_links[i].resolved_use == -1
				        || ((parent_flag && parent_flag->status == atom_use_enum_links[i].resolved_use)
				            || def_status == atom_use_enum_links[i].resolved_use)))
					resolved_status = atom_use_enum_links[i].target_use;
			}
			
			/* Error if flag was forced */
			if (flag->priority == PRIORITY_FORCE && old_status != flag->status)
				portage_die("Explicit flag %s in %s overriden by profile forced flag", flag->name, out->ebuild->ebuild_key);
		}
		
		if (flag) {
			UseFlag* ex_dup = use_new(flag->name, resolved_status, PRIORITY_NORMAL);
			UseReason* reason = use_reason_new(parent, current_use, NULL);
			reason->next = ex_dup->reason;
			ex_dup->reason = reason;
			
			ex_dup->next = out->explicit_flags;
			out->explicit_flags = ex_dup;
		}
	}
	
	return 1;
}