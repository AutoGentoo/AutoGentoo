//
// Created by atuser on 4/23/19.
//

#define _GNU_SOURCE

#include "database.h"
#include "portage_log.h"
#include "directory.h"
#include "dependency.h"
#include "installed_backtrack.h"
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include "language/share.h"
#include <unistd.h>
#include <sys/stat.h>

dependency_t ebuild_installedebuild_cmp(Ebuild* ebuild, InstalledEbuild* installed_ebuild) {
	int cmp_slot = 0;
	int cmp_slot_sub = 0;
	int cmp = 0;
	int cmp_rev = 0;
	
	if (ebuild->slot && installed_ebuild->slot)
		cmp_slot = strcmp(ebuild->slot, installed_ebuild->slot);
	if (cmp_slot != 0)
		return PORTAGE_SLOT;
	
	if (ebuild->sub_slot && installed_ebuild->sub_slot)
		cmp_slot_sub = strcmp(ebuild->sub_slot, installed_ebuild->sub_slot);
	if (cmp_slot_sub != 0)
		return PORTAGE_UPDATE;
	
	cmp = atom_version_compare(ebuild->version, installed_ebuild->version);
	cmp_rev = ebuild->revision - installed_ebuild->revision;
	
	if (cmp > 0)
		return PORTAGE_UPDATE;
	else if (cmp < 0)
		return PORTAGE_DOWNGRADE;
	
	if (cmp_rev > 0)
		return PORTAGE_UPDATE;
	else if (cmp_rev < 0)
		return PORTAGE_DOWNGRADE;
	
	/* Check if the requested use flags match */
	return PORTAGE_REPLACE;
}

InstalledEbuild* portagedb_resolve_installed(PortageDB* db, P_Atom* atom, char* target_slot) {
	InstalledPackage* db_pkg = map_get(db->installed, atom->key);
	if (!db_pkg)
		return NULL;
	
	InstalledEbuild* ebuild;
	for (ebuild = db_pkg->installed; ebuild; ebuild = ebuild->older_slot) {
		int cmp_slot = 0;
		
		if (ebuild->slot && target_slot)
			cmp_slot = strcmp(ebuild->slot, target_slot);
		
		if (cmp_slot != 0)
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
			else if (cmp_rev > 0 && atom->range & ATOM_VERSION_G)
				break;
			else if (cmp_rev < 0 && atom->range & ATOM_VERSION_L)
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
		
		map_insert(db->installed, target->key, target);
	}
	
	InstalledEbuild* ebuild = malloc(sizeof(InstalledEbuild));
	
	ebuild->parent = target;
	ebuild->version = atom->version;
	ebuild->revision = atom->revision;
	ebuild->rebuild_depend = vector_new(VECTOR_UNORDERED | VECTOR_REMOVE);
	ebuild->rebuild_rdepend = vector_new(VECTOR_UNORDERED | VECTOR_REMOVE);
	ebuild->required_by = vector_new(VECTOR_UNORDERED | VECTOR_REMOVE);
	
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
	
	ebuild->cflags = portagedb_ebuild_read(pkg, "CFLAGS");
	ebuild->cxxflags = portagedb_ebuild_read(pkg, "CXXFLAGS");
	ebuild->cbuild = portagedb_ebuild_read(pkg, "CBUILD");
	
	ebuild->depend = NULL;
	ebuild->rdepend = NULL;
	
	char* buf_str = portagedb_ebuild_read(pkg, "DEPEND");
	if (buf_str) {
		ebuild->depend = depend_parse(buf_str);
		backtrack_rebuild_search(db, ebuild, ebuild->depend, EBUILD_REBUILD_DEPEND);
		free(buf_str);
	}
	
	buf_str = portagedb_ebuild_read(pkg, "RDEPEND");
	if (buf_str) {
		ebuild->rdepend = depend_parse(buf_str);
		backtrack_rebuild_search(db, ebuild, ebuild->rdepend, EBUILD_REBUILD_RDEPEND);
		
		/* Perform a dependency backtrack later */
		vector_add(db->backtracking, ebuild);
		free(buf_str);
	}
	
	char* iuse_read = portagedb_ebuild_read(pkg, "IUSE");
	if (iuse_read) {
		ebuild->use = NULL;
		UseFlag* last = NULL;
		UseFlag* use;
		
		for (char* use_tok = strtok(iuse_read, " \n"); use_tok; use_tok = strtok(NULL, " \n")) {
			use = use_new(use_tok, USE_DISABLE, PRIORITY_NORMAL);
			if (!ebuild->use)
				ebuild->use = use;
			else
				last->next = use;
			last = use;
		}
		free(iuse_read);
	}
	
	char* use_temp = portagedb_ebuild_read(pkg, "USE");
	if (use_temp) {
		for (char* use_tok = strtok(use_temp, " \n"); use_tok; use_tok = strtok(NULL, " \n")) {
			UseFlag* target_use = use_get(ebuild->use, use_tok);
			if (!target_use) {
				/* Inside implicit IUSE */
				continue;
			}
			
			target_use->status = USE_ENABLE;
		}
		free(use_temp);
	}
	else
		ebuild->use = NULL;
	
	if (!target->installed)
		target->installed = ebuild;
	else {
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
	out->backtracking = vector_new(VECTOR_REMOVE | VECTOR_UNORDERED);
	out->rebuilds = vector_new(VECTOR_REMOVE | VECTOR_UNORDERED);
	out->blockers = vector_new(VECTOR_REMOVE | VECTOR_UNORDERED);
	
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
		backtrack_rebuild_resolve(out, EBUILD_REBUILD_RDEPEND);
	else
		backtrack_rebuild_resolve(out, EBUILD_REBUILD_RDEPEND | EBUILD_REBUILD_DEPEND);
	
	for (int i = 0; i < out->backtracking->n; i++) {
		backtrack_resolve(out, vector_get(out->backtracking, i));
	}
	
	return out;
}

void portagedb_free(PortageDB* db) {
	free(db->path);
	map_free(db->installed, (void (*) (void*))installedpackage_free);
	
	for (int i = 0; i < db->rebuilds->n; i++)
		backtrack_rebuild_free((RebuildEbuild*) vector_get(db->rebuilds, i));
	vector_free(db->rebuilds);
	
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
	
	use_free(ebuild->use);
	vector_free(ebuild->rebuild_depend);
	vector_free(ebuild->rebuild_rdepend);
	
	free(ebuild);
}