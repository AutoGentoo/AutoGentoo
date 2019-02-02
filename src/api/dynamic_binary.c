//
// Created by atuser on 1/31/19.
//

#include <autogentoo/api/dynamic_binary.h>
#include <stdlib.h>
#include <string.h>
#include <autogentoo/hacksaw/tools.h>

DynamicBinary* dynamic_binary_new() {
	DynamicBinary* out = malloc(sizeof(DynamicBinary));
	out->array_size = NULL;
	
	out->ptr_size = 32;
	out->ptr = malloc(out->ptr_size);
	out->used_size = 0;
	
	out->template_str_size = 16;
	out->template = malloc (out->template_str_size);
	out->template_used_size = 0;
	out->template[0] = 0;
	out->current_template = out->template;
	
	return out;
}

void* prv_dynamic_binary_add_item(DynamicBinary* db, size_t size, void* data_ptr) {
	if (db->used_size + size >= db->ptr_size) {
		db->ptr_size *= 2;
		db->ptr = realloc(db->ptr, db->ptr_size);
	}
	
	void* out = db->ptr + db->used_size;
	memcpy(out, data_ptr, size);
	db->used_size += size;
	
	return out;
}

char* prv_dynamic_binary_template_add(DynamicBinary* db, char type) {
	if (db->template_used_size + 1 >= db->template_str_size) {
		db->template_str_size += 16;
		db->template = realloc(db->template, db->template_str_size);
	}
	
	db->template[db->template_used_size] = type;
	return db->template + ++db->template_used_size;
}

void dynamic_binary_array_start(DynamicBinary* db) {
	prv_dynamic_binary_template_add(db, 'a');
	char* template_end = prv_dynamic_binary_template_add(db, '(');
	
	struct array_node* parent = db->array_size;
	db->array_size = malloc(sizeof(struct array_node));
	db->array_size->template_start = template_end;
	db->array_size->parent = parent;
	
	int current_array_size = 0;
	db->array_size->size_ptr = prv_dynamic_binary_add_item(db, sizeof(int), &current_array_size);
}

void dynamic_binary_array_end(DynamicBinary* db) {
	if (!db->array_size)
		return;
	
	db->current_template = prv_dynamic_binary_template_add(db, ')'); /* Skip over the parenthesis */
	struct array_node* parent = db->array_size->parent;
	free(db->array_size);
	db->array_size = parent;
}

void dynamic_binary_array_next(DynamicBinary* db) {
	db->current_template = db->array_size->template_start;
	(*db->array_size->size_ptr)++;
}

dynamic_bin_t dynamic_binary_add(DynamicBinary* db, char type, void* data) {
	if (*db->current_template) {
		if (type != *db->current_template) {
			lerror("Adding incorrect type. Expected '%c' got '%c'", db, type);
			return DYNAMIC_BIN_ETYPE;
		}
	}
	else
		prv_dynamic_binary_template_add(db, type);
	db->current_template++;
	
	size_t data_size = 0;
	if (type == 'i')
		data_size = sizeof(int);
	else if (type == 's')
		data_size = strlen((char*)data) + 1; // Add the NULL byte
	else if (type == 'v') {
		lerror("Use dynamic_binary_add_binary() instead of dynamic_binary_add()");
		return DYNAMIC_BIN_EBIN;
	}
	
	prv_dynamic_binary_add_item(db, data_size, data);
	return DYNAMIC_BIN_OK;
}

dynamic_bin_t dynamic_binary_add_binary(DynamicBinary* db, size_t n, void* data) {
	if (*db->current_template) {
		if (*db->current_template != 'v') {
			lerror("Tried adding incorrect type. Expected '%c' got '%c'", db, 'v');
			return DYNAMIC_BIN_ETYPE;
		}
	}
	else
		prv_dynamic_binary_template_add(db, 'v');
	db->current_template++;
	
	prv_dynamic_binary_add_item(db, sizeof(size_t), &n);
	prv_dynamic_binary_add_item(db, n, data);
	return DYNAMIC_BIN_OK;
}

void* dynamic_binary_free(DynamicBinary* db) {
	while (db->array_size) {
		lerror("Freeing unclosed array template");
		struct array_node* parent = db->array_size;
		free(db->array_size);
		db->array_size = parent;
	}
	
	void* out = db->ptr;
	free(db->template);
	free(db);
	
	return out;
}