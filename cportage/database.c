//
// Created by atuser on 4/23/19.
//

#include "database.h"
#include "portage_log.h"
#include <string.h>

int database_read_int(Database* db) {
	int i;
	fread(&i, sizeof(int), 1, db->target);
	return i;
}

char* database_read_str(Database* db) {
	size_t s;
	fread(&s, sizeof(size_t), 1, db->target);
	if (s == 0)
		return NULL;
	
	char* out = malloc(s);
	fread(out, 1, s, db->target);
	return out;
}

long database_read_long(Database* db) {
	long out;
	fread(&out, sizeof(long), 1, db->target);
	return out;
}

void database_write_str(Database* db, char* str) {
	size_t s = 0;
	if (!str) {
		fwrite(&s, sizeof(size_t), 1, db->target);
		return;
	}
	
	s = strlen(str)+ 1;
	fwrite(&s, sizeof(size_t), 1, db->target);
	fwrite(str, s, 1, db->target);
}

void database_write_int(Database* db, int i) {
	fwrite(&i, sizeof(int), 1, db->target);
}

void database_write_long(Database* db, long l) {
	fwrite(&l, sizeof(long), 1, db->target);
}

void database_write_dependency(Database* db, Dependency* dp) {
	if (!dp)
		return database_write_int(db, DATABASE_STOP);
	database_write_int(db, DATABASE_DEPEND);
	
	database_write_str(db, dp->target);
	database_write_int(db, dp->selector);
	database_write_int(db, dp->depends);
	
	database_write_dependency(db, dp->selectors);
	database_write_dependency(db, dp->next);
}

Dependency* database_read_dependency(Database* db) {
	database_t action = database_read_int(db);
	if (action == DATABASE_STOP)
		return NULL;
	
	Dependency* out = malloc(sizeof(Dependency));
	out->target = database_read_str(db);
	out->selector = database_read_int(db);
	out->depends = database_read_int(db);
	
	out->selectors = database_read_dependency(db);
	out->next = database_read_dependency(db);
	
	return out;
}

Vector* database_read_vector(Database* db) {
	size_t el_size = database_read_int(db);
	vector_opts opts = database_read_int(db);
	int n = database_read_int(db);
	char* template = database_read_str(db);
	size_t item_size = database_read_int(db);
	
	Vector* out_ptr = malloc(sizeof(Vector));
	out_ptr->s = n + 1;
	out_ptr->size = el_size;
	out_ptr->ptr = malloc(out_ptr->size * out_ptr->s);
	out_ptr->n = n;
	out_ptr->opts = opts;
	
	for (int i = 0; i < n; i++) {
		void* item = malloc(item_size);
		size_t current_offset = 0;
		char* str_temp;
		int int_temp;
		long long_temp;
		
		for (char* t = template; *t; t++) {
			if (*t == 's') {
				str_temp = database_read_str(db);
				memcpy(item + current_offset, &str_temp, sizeof(char*));
				current_offset += sizeof(char*);
			}
			else if (*t == 'i') {
				int_temp = database_read_int(db);
				memcpy(item + current_offset, &int_temp, sizeof(int));
				current_offset += sizeof(int);
			}
			else if (*t == 'l') {
				long_temp = database_read_long(db);
				memcpy(item + current_offset, &long_temp, sizeof(long));
				current_offset += sizeof(long);
			}
		}
		
		vector_add(out_ptr, &item);
	}
	
	return out_ptr;
}

void database_write_vector(Database* db, Vector* vec, char* template) {
	database_write_int(db, vec->size);
	database_write_int(db, vec->opts);
	database_write_int(db, vec->n);
	database_write_str(db, template);
	
	size_t item_size = 0;
	for (char* t = template; *t; t++) {
		if (*t == 's')
			item_size += sizeof(char*);
		else if (*t == 'i')
			item_size += sizeof(int);
		else if (*t == 'l')
			item_size += sizeof(long);
	}
	
	database_write_int(db, item_size);
	
	for (int i = 0; i < vec->n; i++) {
		void** item = vector_get(vec, i);
		size_t current_offset = 0;
		
		for (char* t = template; *t; t++) {
			if (*t == 's') {
				database_write_str(db, item[current_offset]);
				current_offset += sizeof(char*);
			}
			else if (*t == 'i') {
				database_write_int(db, ((int*)item[current_offset])[0]);
				current_offset += sizeof(int);
			}
			else if (*t == 'l') {
				database_write_long(db, (long)item[current_offset]);
				current_offset += sizeof(long);
			}
		}
	}
}

void database_write_map(Database* db, Map* m) {
	StringVector* keys = map_all_keys(m);
	database_write_int(db, m->size);
	database_write_long(db, m->threshold);
	database_write_int(db, keys->n);
	
	char* key;
	Package* pkg;
	for (int i = 0; i < keys->n; i++) {
		key = string_vector_get(keys, i);
		if (!(pkg = map_get(m, key))) {
			plog_error("Key %s not found in map", key);
			continue;
		}
		
		database_write_str(db, key);
		database_write_int(db, pkg->hash);
		database_write_str(db, pkg->category);
		database_write_str(db, pkg->name);
		
		for (Ebuild* current = pkg->ebuilds;; current = current->older) {
			if (!current) {
				database_write_int(db, DATABASE_STOP);
				break;
			}
			database_write_int(db, DATABASE_EBUILD);
			
			database_write_str(db, current->pn);
			database_write_str(db, current->pv);
			database_write_str(db, current->pr);
			database_write_str(db, current->slot);
			database_write_str(db, current->eapi);
			
			/* Cached in the database */
			database_write_dependency(db, current->depend);
			database_write_dependency(db, current->bdepend);
			database_write_dependency(db, current->rdepend);
			database_write_dependency(db, current->pdepend);
			
			database_write_vector(db, current->use, "sii");
			database_write_vector(db, current->feature_restrict, "i");
			fwrite(current->keywords, sizeof(int), ARCH_END, db->target);
			
			//database_write_dependency(db, current->required_use);
			database_write_dependency(db, current->src_uri);
		}
	}
}

Map* database_read_map(Database* db) {
	size_t size = database_read_int(db);
	double threshold = database_read_long(db);
	Map* out = map_new(size, threshold);
	
	int n = database_read_int(db);
	
	char* key;
	for (int i = 0; i < n; i++) {
		Package* pkg = malloc(sizeof(Package));
		
		key = database_read_str(db);
		pkg->key = key;
		pkg->hash = database_read_int(db);
		pkg->category = database_read_str(db);
		pkg->name = database_read_str(db);
		
		Ebuild* newer = NULL;
		// EBUILDS
		for (database_t j = database_read_int(db); j == DATABASE_EBUILD; j = database_read_int(db)) {
			Ebuild* current = malloc(sizeof(Ebuild));
			current->pn = database_read_str(db);
			current->pv = database_read_str(db);
			current->pr = database_read_str(db);
			current->slot = database_read_str(db);
			current->eapi = database_read_str(db);
			
			/* Cached in the database */
			current->depend = database_read_dependency(db);
			current->bdepend = database_read_dependency(db);
			current->rdepend = database_read_dependency(db);
			current->pdepend = database_read_dependency(db);
			
			database_write_vector(db, current->use, "sii");
			database_write_vector(db, current->feature_restrict, "i");
			fwrite(current->keywords, sizeof(int), ARCH_END, db->target);
			
			//database_write_dependency(db, current->required_use);
			database_write_dependency(db, current->src_uri);
			
			current->newer = newer;
			if (current->newer)
				current->newer->older = current;
			newer = current;
		}
		
		map_insert(out, pkg->key, pkg);
	}
	
	return out;
}

void database_read(Database* db) {
	if (!db->target) {
		db->target = fopen("autogentoo.db", "r");
		if (!db->target) {
			plog_error("Failed to open database file");
			exit(1);
		}
	}
	
	if (!db->repo) {
		db->repo = malloc(sizeof(Repository));
	}
	
	db->repo->name = database_read_str(db);
	db->repo->hash_make_conf = (unsigned char*)database_read_str(db);
	db->repo->hash_accept_keywords = (unsigned char*)database_read_str(db);
	db->repo->hash_package_use = (unsigned char*)database_read_str(db);
	
	db->repo->packages = database_read_map(db);
}

void database_write(Database* db) {
	if (db->target)
		fclose(db->target);
	db->target = fopen("autogentoo.db", "w+");
	
	database_write_str(db, db->repo->name);
	database_write_str(db, (char*)db->repo->hash_make_conf);
	database_write_str(db, (char*)db->repo->hash_accept_keywords);
	database_write_str(db, (char*)db->repo->hash_package_use);
	database_write_map(db, db->repo->packages);
}