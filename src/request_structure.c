//
// Created by atuser on 4/7/18.
//

#include <string.h>
#include <stdlib.h>
#include <autogentoo/request_structure.h>

int prv_handle_item (void* dest, void* data, char type, size_t item_size, void* end) {
	if (data + item_size > end)
		return -1;
	
	if (type == 's') {
		char* t = strdup(data);
		data = &t;
	}
	
	memcpy (dest, data, item_size);
	
	return (int)item_size;
}

int prv_handle_array (void* dest, void* data, char type, size_t item_size, void* end) {
	int len = 0;
	for (int i = 0; memcmp(data, 0, item_size) != 0; data += (++i + item_size)) {
		int temp = prv_handle_item(dest + (i * item_size), data, type, item_size, end);
		if (temp == -1)
			return -1;
		len += temp;
	}
	
	
	return len;
}

int parse_request_structure(RequestData* out, request_structure_t type, void* data, void* end) {
	char* template = request_structure_linkage[type - 1];
	void* ptr = out;
	void* old = data;
	
	char* i = template;
	for (; i != NULL; i++) {
		int len = 0;
		
		struct {
			char type;
			size_t size;
		} type_sizes[] = {
				{'c', sizeof(char)},
				{'i', sizeof(int)},
				{'l', sizeof(long)},
				{'s', sizeof(char*)},
		};
		
		if (*i == 'a')
			i += 2;
		
		size_t item_size = 1;
		for (int j = 0; j < sizeof(type_sizes) / sizeof(type_sizes[0]); j++)
			if (*i == type_sizes[j].type)
				item_size = type_sizes[j].size;
		
		if (*i == 'a') {
			int array_length = *(int*)data;
			data += sizeof (int);
			
			memcpy (ptr, malloc (array_length * item_size), sizeof(void*));
			len = prv_handle_array(ptr, data, *i, item_size, end);
		}
		else
			len = prv_handle_item (ptr, data, *i, item_size, end);
		
		if (len == -1)
			return -1;
		
		ptr += len;
		data += len;
	}
	
	return (int)(data - old);
}

void free_request_structure(RequestData* to_free, request_structure_t type) {
	char* template = request_structure_linkage[type - 1];
	char* i = template;
	
	size_t offset = 0;
	for (; i != NULL; i++) {
		struct {
			char type;
			size_t size;
		} type_sizes[] = {
				{'c', sizeof(char)},
				{'i', sizeof(int)},
				{'l', sizeof(long)},
				{'s', sizeof(char*)},
		};
		size_t item_size = 1;
		for (int j = 0; j < sizeof(type_sizes) / sizeof(type_sizes[0]); j++)
			if (*i == type_sizes[j].type)
				item_size = type_sizes[j].size;
		
		if (*i == 's')
			free (to_free + offset);
		
		offset += item_size;
	}
}