//
// Created by atuser on 4/23/19.
//

#define _GNU_SOURCE

#include "database.h"
#include "portage_log.h"
#include "directory.h"
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <share.h>
#include <unistd.h>
#include <sys/stat.h>

int ebuild_installedebuild_cmp(Ebuild* ebuild, InstalledEbuild* installed_ebuild) {
	int cmp_slot = 0;
	int cmp_slot_sub = 0;
	int cmp = 0;
	int cmp_rev = 0;
	
	if (ebuild->slot && installed_ebuild->slot)
		cmp_slot = strcmp(ebuild->slot, installed_ebuild->slot);
	if (cmp_slot != 0)
		return cmp_slot;
	
	if (ebuild->sub_slot && installed_ebuild->sub_slot)
		cmp_slot_sub = strcmp(ebuild->sub_slot, installed_ebuild->sub_slot);
	if (cmp_slot_sub != 0)
		return cmp_slot_sub;
	
	cmp = atom_version_compare(ebuild->version, installed_ebuild->version);
	cmp_rev = ebuild->revision - installed_ebuild->revision;
	
	if (cmp != 0)
		return cmp;
	if (cmp_rev != 0)
		return cmp_rev;
	
	return 0;
}

InstalledEbuild* portagedb_resolve_installed(PortageDB* db, P_Atom* atom) {
	InstalledPackage* db_pkg = map_get(db->installed, atom->key);
	if (!db_pkg)
		return NULL;
	
	InstalledEbuild* ebuild;
	for (ebuild = db_pkg->installed; ebuild; ebuild = ebuild->older_slot) {
		int cmp_slot = 0;
		int cmp_slot_sub = 0;
		
		if (ebuild->slot && atom->slot)
			cmp_slot = strcmp(ebuild->slot, atom->slot);
		if (ebuild->sub_slot && atom->sub_slot)
			cmp_slot_sub = strcmp(ebuild->sub_slot, atom->sub_slot);
		
		if (cmp_slot != 0 || cmp_slot_sub != 0)
			continue;
		
		int cmp = 0;
		int cmp_rev = 0;
		if (atom->version) {
			cmp = atom_version_compare(ebuild->version, atom->version);
			cmp_rev = ebuild->revision - atom->revision;
		}
		
		if (cmp == 0 && atom->range & ATOM_VERSION_E) {
			if (atom->range == ATOM_VERSION_E) {
				if (cmp_rev == 0)
					break;
			}
			else if (atom->range == ATOM_VERSION_REV)
				break;
			
			if (cmp_rev == 0)
				break;
			else if (cmp_rev > 0 && atom->range & ATOM_VERSION_L)
				break;
		else if (cmp_rev < 0 && atom->range & ATOM_VERSION_G)
				break;
		}
		
		if (cmp < 0 && atom->range & ATOM_VERSION_L) {
			break;
		}
		if (cmp > 0 && atom->range & ATOM_VERSION_G)
			break;
	}
	
	return ebuild;
}

char* portagedb_ebuild_read(FPNode* pkg, char* property) {
	if (pkg->type != FP_NODE_DIR)
		return NULL;
	
	char* prop_path = NULL;
	asprintf(&prop_path, "%s/%s", pkg->path, property);
	int fd = open(prop_path, O_RDONLY);
	free(prop_path);
	if (fd < 0) {
		//plog_error("Could not read property %s from package %s", property, pkg->path);
		return NULL;
	}
	
	off_t size = lseek(fd, 0, SEEK_END);
	char* out = malloc(size + 1);
	lseek(fd, 0, SEEK_SET);
	read(fd, out, size);
	out[size] = 0;
	
	close(fd);
	
	return out;
}

void portagedb_add_ebuild(PortageDB* db, FPNode* cat, FPNode* pkg) {
	char* atom_key;
	asprintf(&atom_key, "%s/%s", cat->filename, pkg->filename);
	
	P_Atom* atom = atom_parse(atom_key);
	if (!atom) {
		plog_warn("Could not parse atom %s", atom_key);
		free(atom_key);
		return;
	}
	
	free(atom_key);
	InstalledPackage* target = map_get(db->installed, atom->key);
	if (!target) {
		target = malloc(sizeof(InstalledPackage));
		target->key = strdup(atom->key);
		target->installed = NULL;
		target->name = strdup(atom->name);
		target->category = strdup(atom->category);
		
		target->hash = map_insert(db->installed, target->key, target);
	}
	
	InstalledEbuild* ebuild = malloc(sizeof(InstalledEbuild));
	
	ebuild->parent = target;
	ebuild->version = atom->version;
	ebuild->revision = atom->revision;
	ebuild->rebuild_depend = vector_new(sizeof(RebuildEbuild*), VECTOR_UNORDERED | VECTOR_REMOVE);
	ebuild->rebuild_rdepend = vector_new(sizeof(RebuildEbuild*), VECTOR_UNORDERED | VECTOR_REMOVE);
	
	atomflag_free(atom->useflags);
	free(atom->key);
	
	if (atom->slot)
		free(atom->slot);
	if (atom->sub_slot)
		free(atom->sub_slot);
	free(atom->repository);
	free(atom->category);
	free(atom->name);
	free(atom);
	
	ebuild->slot = NULL;
	ebuild->sub_slot = NULL;
	
	char* slot_temp = portagedb_ebuild_read(pkg, "SLOT");
	if (slot_temp) {
		char* tok1 = strtok(slot_temp, "/\n");
		if (tok1) {
			ebuild->slot = strdup(tok1);
			char* tok2 = strtok(NULL, "/\n");
			if (tok2)
				ebuild->sub_slot = strdup(tok2);
		}
		
		free(slot_temp);
	}
	
	ebuild->older_slot = NULL;
	ebuild->newer_slot = NULL;
	ebuild->next_required = NULL;
	ebuild->required_head = NULL;
	
	ebuild->cflags = portagedb_ebuild_read(pkg, "CFLAGS");
	ebuild->cxxflags = portagedb_ebuild_read(pkg, "CXXFLAGS");
	ebuild->cbuild = portagedb_ebuild_read(pkg, "CBUILD");
	
	ebuild->depend = NULL;
	ebuild->rdepend = NULL;
	
	char* buf_str = portagedb_ebuild_read(pkg, "DEPEND");
	if (buf_str) {
		ebuild->depend = depend_parse(buf_str);
		backtrack_search(db, ebuild, ebuild->depend, EBUILD_REBUILD_DEPEND);
		free(buf_str);
	}
	
	buf_str = portagedb_ebuild_read(pkg, "RDEPEND");
	if (buf_str) {
		ebuild->rdepend = depend_parse(buf_str);
		backtrack_search(db, ebuild, ebuild->rdepend, EBUILD_REBUILD_RDEPEND);
		free(buf_str);
	}
	
	char* use_temp = portagedb_ebuild_read(pkg, "USE");
	if (use_temp) {
		ebuild->use = NULL;
		UseFlag* last = NULL;
		UseFlag* use;
		
		for (char* use_tok = strtok(use_temp, " \n"); use_tok; use_tok = strtok(NULL, " \n")) {
			use = useflag_new(use_tok, USE_ENABLE);
			if (!ebuild->use)
				ebuild->use = use;
			else
				last->next = use;
			last = use;
		}
		free(use_temp);
	}
	else
		ebuild->use = NULL;
	
	if (!target->installed)
		target->installed = ebuild;
	else {
		if (strcmp(ebuild->slot, "4.14.78") == 0) {
			char* d = NULL;
		}
		
		for (InstalledEbuild* head = target->installed;; head = head->older_slot) {
			int slot_cmp = strcmp(head->slot, ebuild->slot);
			int sub_slot_cmp = 0;
			
			if (head->sub_slot && !ebuild->sub_slot)
				sub_slot_cmp = 1;
			else if (!head->sub_slot && ebuild->sub_slot)
				sub_slot_cmp = -1;
			else if (head->sub_slot && ebuild->sub_slot)
				sub_slot_cmp = strcmp(head->sub_slot, ebuild->sub_slot);
			
			if (slot_cmp < 0 || (slot_cmp == 0 && sub_slot_cmp < 0)) {
				ebuild->newer_slot = head->newer_slot;
				ebuild->older_slot = head;
				head->newer_slot = ebuild;
				if (head == target->installed)
					target->installed = ebuild;
				else
					ebuild->newer_slot->older_slot = ebuild;
				break;
			}
			if (!head->older_slot) {
				head->older_slot = ebuild;
				ebuild->newer_slot = head;
				break;
			}
		}
	}
}

PortageDB* portagedb_read(Emerge* emerge) {
	PortageDB* out = malloc(sizeof(PortageDB));
	
	asprintf(&out->path, "%svar/db/pkg", emerge->root);
	int db_dir = open(out->path, O_RDONLY);
	if (db_dir < 0) {
		plog_error("Failed to open database directory: %s", out->path);
		free(out->path);
		free(out);
		return NULL;
	}
	FPNode* old;
	
	out->installed = map_new(4196, 0.8);
	out->backtracking = vector_new(sizeof(RebuildEbuild*), VECTOR_REMOVE | VECTOR_UNORDERED);
	int ebuild_n = 0;
	emerge->database = out;
	
	FPNode* categories = open_directory_stat(__S_IFDIR, db_dir, out->path, NULL);
	close(db_dir);
	for (FPNode* cat = categories; cat;) {
		if (cat->type != FP_NODE_DIR) {
			plog_warn("Database category %s is not a directory", cat->path);
			continue;
		}
		
		int cat_dirfd = open(cat->path, O_RDONLY);
		if (cat_dirfd < 0) {
			plog_error("Failed to open database category %s", cat->filename);
			continue;
		}
		
		FPNode* pkgs = open_directory_stat(__S_IFDIR, db_dir, cat->parent_dir, cat->filename);
		close(cat_dirfd);
		for (FPNode* pkg = pkgs; pkg;) {
			portagedb_add_ebuild(out, cat, pkg);
			ebuild_n++;
			free(pkg->path);
			free(pkg->filename);
			free(pkg->parent_dir);
			
			old = pkg;
			pkg = pkg->next;
			free(old);
		}
		
		free(cat->path);
		free(cat->filename);
		free(cat->parent_dir);
		old = cat;
		cat = cat->next;
		free(old);
	}
	
	if (EMERGE_USE_BINHOST & emerge->build_opts)
		backtrack_resolve(out, EBUILD_REBUILD_RDEPEND);
	else
		backtrack_resolve(out, EBUILD_REBUILD_RDEPEND | EBUILD_REBUILD_DEPEND);
	
	return out;
}

void backtrack_search(PortageDB* db, InstalledEbuild* parent, Dependency* deptree, rebuild_t type) {
	if (!deptree)
		return;
	
	Dependency* next;
	while (deptree) {
		next = deptree->next;
		backtrack_search(db, parent, deptree->selectors, type);
		if (deptree->depends == IS_ATOM && deptree->atom->sub_opts == ATOM_SLOT_REBUILD)
			backtrack_new(db, parent, deptree->atom, type);
		deptree = next;
	}
}

void backtrack_new(PortageDB* db, InstalledEbuild* rebuild, P_Atom* atom, rebuild_t type) {
	RebuildEbuild* backtrack = malloc(sizeof(RebuildEbuild));
	backtrack->new_slot = NULL;
	backtrack->old_slot = NULL; //!< Needs to be resolved;
	backtrack->rebuild = rebuild;
	backtrack->selector = atom_dup(atom);
	backtrack->type = type;
	
	vector_add(db->backtracking, &backtrack);
}

void backtrack_resolve(PortageDB* db, rebuild_t types) {
	for (int i = 0; i < db->backtracking->n; i++) {
		RebuildEbuild* backtrack = *(RebuildEbuild**)vector_get(db->backtracking, i);
		if (!(backtrack->type & types))
			continue;
		
		InstalledPackage* package = map_get(db->installed, backtrack->selector->key);
		if (!package) {
			plog_warn("Invalid backtracking request for package %s (%s)", backtrack->rebuild->parent->key, backtrack->selector->key);
			plog_warn("Is %s a binary package?", backtrack->rebuild->parent->key);
			continue;
		}
		
		InstalledEbuild* curr;
		for (curr = package->installed; curr; curr = curr->older_slot) {
			if (backtrack->selector->slot && strcmp(curr->slot, backtrack->selector->slot) != 0)
				continue;
			backtrack->old_slot = curr;
			
			if (backtrack->type == EBUILD_REBUILD_DEPEND)
				vector_add(curr->rebuild_depend, &backtrack);
			if (backtrack->type == EBUILD_REBUILD_RDEPEND)
				vector_add(curr->rebuild_rdepend, &backtrack);
		}
	}
}

void backtrack_free(RebuildEbuild* rebuild) {
	atom_free(rebuild->selector);
	free(rebuild);
}

void portagedb_free(PortageDB* db) {
	free(db->path);
	map_free(db->installed, (void (*) (void*))installedpackage_free);
	
	for (int i = 0; i < db->backtracking->n; i++)
		backtrack_free(*(RebuildEbuild**)vector_get(db->backtracking, i));
	vector_free(db->backtracking);
	
	free(db);
}

void installedpackage_free(InstalledPackage* pkg) {
	InstalledEbuild* next = NULL;
	InstalledEbuild* eb = pkg->installed;
	while(eb) {
		next = eb->older_slot;
		installedebuild_free(eb);
		eb = next;
	}
	free(pkg->key);
	free(pkg->name);
	free(pkg->category);
	
	free(pkg);
}

static int ebuild_freed = 0;

void installedebuild_free(InstalledEbuild* ebuild) {
	dependency_free(ebuild->depend);
	dependency_free(ebuild->rdepend);
	atomversion_free(ebuild->version);
	if (ebuild->slot)
		free(ebuild->slot);
	if (ebuild->sub_slot)
		free(ebuild->sub_slot);
	if (ebuild->cflags)
		free(ebuild->cflags);
	if (ebuild->cxxflags)
		free(ebuild->cxxflags);
	if (ebuild->cbuild)
		free(ebuild->cbuild);
	
	useflag_free(ebuild->use);
	vector_free(ebuild->rebuild_depend);
	vector_free(ebuild->rebuild_rdepend);
	
	free(ebuild);
}