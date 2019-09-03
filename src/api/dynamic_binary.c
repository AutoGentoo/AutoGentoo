//
// Created by atuser on 1/31/19.
//

#include <autogentoo/api/dynamic_binary.h>
#include <stdlib.h>
#include <string.h>
#include <autogentoo/hacksaw/hacksaw.h>
#include <autogentoo/endian_convert.h>

DynamicBinary* dynamic_binary_new(dynamic_binary_endian_t endian) {
	DynamicBinary* out = malloc(sizeof(DynamicBinary));
	out->array_size = NULL;
	
	out->ptr_size = 32;
	out->ptr = malloc(out->ptr_size);
	out->used_size = 0;
	
	out->template_str_size = 16;
	out->template = calloc (out->template_str_size, 1);
	out->template_used_size = 0;
	out->current_template = out->template;
	
	out->endian = endian;
	
	return out;
}

void* prv_dynamic_binary_add_item(DynamicBinary* db, size_t size, void* data_ptr) {
	if (!size)
		return db->ptr;
	
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
	if (db->template_used_size + 2 >= db->template_str_size) {
		db->template_str_size += 16;
		db->template = realloc(db->template, db->template_str_size);
	}
	
	if (*db->current_template == 0) { // Adding to the end
		db->template_used_size++;
		db->current_template[1] = 0; // NULL end of string
	}
	else
		if (*db->current_template != type) {
			lerror("Tried adding incorrect type. Expected '%c' got '%c'", *db->current_template, type);
			return NULL;
		}
	
	*db->current_template = type;
	db->current_template++;
	
	return db->current_template;
}

void dynamic_binary_array_start(DynamicBinary* db) {
	prv_dynamic_binary_template_add(db, 'a');
	prv_dynamic_binary_template_add(db, '(');
	
	struct array_node* parent = db->array_size;
	db->array_size = malloc(sizeof(struct array_node));
	db->array_size->template_start = db->current_template;
	db->array_size->parent = parent;
	db->array_size->size_offset = db->used_size;
	db->array_size->size = 0;
	
	int current_array_size = 0;
	prv_dynamic_binary_add_item(db, sizeof(int), &current_array_size);
}

int dynamic_binary_array_end(DynamicBinary* db) {
	if (!db->array_size)
		return 0;
	
	if (db->array_size->size != 0)
		db->current_template = db->array_size->template_end;
	prv_dynamic_binary_template_add(db, ')');
	struct array_node* parent = db->array_size->parent;
	
	int size_temp = db->array_size->size;
	if (db->endian & DB_ENDIAN_TARGET_NETWORK && !(db->endian & DB_ENDIAN_INPUT_NETWORK))
		size_temp = htonl((uint32_t)size_temp);
	memcpy(db->ptr + db->array_size->size_offset, &size_temp, sizeof(int));
	int out = db->array_size->size;
	
	free(db->array_size);
	db->array_size = parent;
	
	return out;
}

void dynamic_binary_array_next(DynamicBinary* db) {
	db->array_size->template_end = db->current_template;
	db->current_template = db->array_size->template_start;
	db->array_size->size++;
}

dynamic_bin_t dynamic_binary_add(DynamicBinary* db, char type, void* data) {
	if (prv_dynamic_binary_template_add(db, type) == NULL)
		return DYNAMIC_BIN_ETYPE;
	
	size_t data_size = 0;
	if (type == 'i') {
		data_size = sizeof(int);
		int* src_data = data;
		data = malloc (sizeof(int));
		*(int*)data = *src_data;
		if (db->endian & DB_ENDIAN_TARGET_NETWORK && !(db->endian & DB_ENDIAN_INPUT_NETWORK))
			*(int*)data = htonl(*(uint32_t*)data);
		else if (!(db->endian & DB_ENDIAN_TARGET_NETWORK) && db->endian & DB_ENDIAN_INPUT_NETWORK)
			*(int*)data = ntohl(*(uint32_t*)data);
	}
	else if (type == 's') {
		if (data)
			data_size = strlen((char*)data);
		
		dynamic_binary_add(db, 'i', &data_size);
	}
	else if (type == 'v') {
		lerror("Use dynamic_binary_add_binary() instead of dynamic_binary_add()");
		return DYNAMIC_BIN_EBIN;
	}
	
	prv_dynamic_binary_add_item(db, data_size, data);
	return DYNAMIC_BIN_OK;
}

dynamic_bin_t dynamic_binary_add_binary(DynamicBinary* db, size_t n, void* data) {
	if (prv_dynamic_binary_template_add(db, 'v') == NULL)
		return DYNAMIC_BIN_ETYPE;
	
	prv_dynamic_binary_add_item(db, sizeof(size_t), &n);
	prv_dynamic_binary_add_item(db, n, data);
	return DYNAMIC_BIN_OK;
}

dynamic_bin_t dynamic_binary_add_quick(DynamicBinary* db, char* template, DynamicType* content) {
	int i = 0;
	for (char* c = template; *c; c++, i++) {
		if (*c == 'i')
			dynamic_binary_add(db, 'i', &content[i].integer);
		else if (*c == 's')
			dynamic_binary_add(db, 's', content[i].string);
		else if (*c == 'v')
			dynamic_binary_add_binary(db, content[i].binary.n, content[i].binary.data);
		else if (*c == 'a') {
			i--;
			dynamic_binary_array_start(db);
		}
		else if (*c == 'n') {
			i--;
			dynamic_binary_array_next(db);
		}
		else if (*c == 'e') {
			i--;
			dynamic_binary_array_next(db);
			dynamic_binary_array_end(db);
		}
	}
	
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