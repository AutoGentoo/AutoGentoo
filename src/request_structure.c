//
// Created by atuser on 4/7/18.
//

#include <string.h>
#include <stdlib.h>
#include <autogentoo/request_structure.h>
#include <netinet/in.h>

int prv_handle_item (void* dest, void* data, char type, size_t item_size, void* end) {
	if (data + item_size > end)
		return -1;
	
	if (type == 's') {
		char* t = strdup(data);
		data = &t;
	}
	
	memcpy (dest, data, item_size);
	
	if (type == 'i')
		*(int*)dest = ntohl((uint32_t) (*(int*)dest));
	
	return (int)item_size;
}

int memcmp_c (void* __s1, char c, size_t size) {
	for (int i = 0; i < size; i++)
		if (((char*)__s1)[0] != c)
			return 1;
	
	return 0;
}

int prv_handle_array (void* dest, void* data, char type, size_t item_size, int count, void* end) {
	int len = 0;
	for (int i = 0; i < count; data += (++i + item_size)) {
		int temp = prv_handle_item(dest + (i * item_size), data, type, item_size, end);
		if (temp == -1)
			return -1;
		len += temp;
	}
	
	return len;
}

int parse_request_structure (RequestData* out, char* template, void* data, void* end) {
	void* ptr = out;
	void* old = data;
	
	char* i = template;
	for (; i != NULL; i++) {
		int len = 0;
		
		if (*i == 'a') {
			i += 2;
			int array_length = ntohl((uint32_t) (*(int*)data));
			memcpy(ptr, &array_length, sizeof (int));
			data += sizeof (int);
			ptr += sizeof (int);
			
			char* i_next  = strchr(i, ')');
			*i_next = 0;
			
			*(void**) ptr = malloc (get_ssizeof(i) * array_length);
			
			for (int j = 0; j < array_length; j++) {
				len = parse_request_structure(((void**)ptr)[j], i, data, end);
				ptr += len;
				data += len;
				
				if (len == -1) {
					for (--j; j >= 0; j--)
						free_request_structure(((void**)ptr)[j], i, NULL);
					free (ptr);
					free_request_structure (out, template, i);
					return -1;
				}
			}
			
			i = i_next + 1;
			continue;
		}
		
		size_t item_size = get_sizeof(*i);
		len = prv_handle_item (ptr, data, *i, item_size, end);
		
		if (len == -1)
			return -1;
		
		ptr += len;
		data += len;
	}
	
	return (int)(data - old);
}

size_t get_sizeof (char c) {
	struct {
		char type;
		size_t size;
	} type_sizes[] = {
			{'c', sizeof(char)},
			{'i', sizeof(int)},
			{'l', sizeof(long)},
			{'s', sizeof(char*)},
			{'a', sizeof(void*)},
	};
	
	size_t item_size = 1;
	for (int j = 0; j < sizeof(type_sizes) / sizeof(type_sizes[0]); j++)
		if (c == type_sizes[j].type)
			item_size = type_sizes[j].size;
	
	return item_size;
}

size_t get_ssizeof (char* template) {
	size_t out = 0;
	for (char* i = template; i != NULL; i++)
		out += get_sizeof(*i);
	return out;
}

void free_request_structure (RequestData* to_free, char* template, const char* end) {
	char* i = template;
	
	size_t offset = 0;
	for (; i != NULL; i++) {
		if (end && i >= end)
			break;
		
		if (*i == 'a') {
			i += 2;
			
			int array_length = *(int*) (to_free + offset);
			offset += sizeof (int);
			
			char* i_next  = strchr(i, ')');
			*i_next = 0;
			
			for (int j = 0; j < array_length; j++)
				free_request_structure((*(void***)(to_free + offset))[j], i, NULL);
			
			i = i_next + 1;
			offset += sizeof (void*);
			
			continue;
		}
		
		size_t item_size = get_sizeof(*i);
		
		if (*i == 's')
			free (to_free + offset);
		
		
		offset += item_size;
	}
}