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

InstalledEbuild* portagedb_resolve_installed(PortageDB* db, P_Atom* atom) {

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

static int ebuild_alloc = 0;

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
	printf("\r%d", ++ebuild_alloc);
	
	ebuild->parent = target;
	ebuild->version = atom->version;
	ebuild->slot = NULL;
	ebuild->sub_slot = NULL;
	
	char* slot_temp = portagedb_ebuild_read(pkg, "SLOT");
	if (slot_temp) {
		char* tok1 = strtok(slot_temp, "/\n");
		if (tok1) {
			ebuild->slot = strdup(tok1);
			char* tok2 = strtok(slot_temp, "/\n");
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
	
	struct {
		char* prop;
		Dependency** atom;
	} depend_expr[] = {
			{"DEPEND", &ebuild->depend},
			{"RDEPEND", &ebuild->rdepend},
			{NULL, NULL}
	};
	
	for (int i = 0; depend_expr[i].prop; i++) {
		char* buf_str = portagedb_ebuild_read(pkg, depend_expr[i].prop);
		if (!buf_str)
			continue;
		if (buf_str[0] == '\n') {
			free(buf_str);
			continue;
		}
		*(depend_expr[i].atom) = depend_parse(buf_str);
		free(buf_str);
	}
	
	//char* iuse_temp = portagedb_ebuild_read(ebuild, pkg, "IUSE_EFFECTIVE");
	char* use_temp = portagedb_ebuild_read(pkg, "USE");
	if (use_temp) {
		char* use_tok = strtok(use_temp, " \n");
		UseFlag* use = ebuild->use = useflag_new(use_tok, USE_ENABLE);
		for (use_tok = strtok(NULL, " \n"); use_tok; use_tok = strtok(NULL, " \n")) {
			use->next = useflag_new(use_tok, USE_DISABLE);
			use = use->next;
		}
		use->next = NULL;
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
			if (!head->sub_slot && head->sub_slot != ebuild->sub_slot)
				plog_warn("%s-%s has subslot but %s does not", head->parent->key, head->version->full_version, ebuild->parent->key, ebuild->version->full_version);
			else
				sub_slot_cmp = strcmp(head->sub_slot, ebuild->sub_slot);
			
			if (slot_cmp < 0 || (slot_cmp == 0 && sub_slot_cmp < 0)) {
				ebuild->newer_slot = head->newer_slot;
				ebuild->older_slot = head;
				head->newer_slot = ebuild;
				if (head == target->installed)
					target->installed = ebuild;
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
	printf(" alloced\n");
	return out;
}


void portagedb_free(PortageDB* db) {
	free(db->path);
	map_free(db->installed, (void (*) (void*))installedpackage_free);
	free(db);
	printf(" freed\n");
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
	printf("\r%d", ++ebuild_freed);
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
	
	free(ebuild);
}