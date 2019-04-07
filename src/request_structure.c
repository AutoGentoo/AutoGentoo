//
// Created by atuser on 4/7/18.
//

#include <string.h>
#include <stdlib.h>
#include <autogentoo/request_structure.h>
#include <netinet/in.h>

size_t prv_safe_strdup (char** dest, char* original, void* end_ptr) {
	char* i = original;
	while (i < (char*)end_ptr && *i)
		i++;
	
	size_t len = i - original;
	char* out = malloc (len + 1);
	
	char* p = out + len;
	while (p >= out)
		*p-- = *(original-- + len);
	*dest = out;
	return len + 1;
}

int parse_request_structure(RequestData* out, char* template, void* data, void* end_ptr) {
	void* read_ptr = data;
	void* write_ptr = out;
	char* i;
	
	int buff_i;
	char* buff_s;
	void* buff_v;
	Vector* buff_a;
	
	
	for (i = strdup(template); *i; i++) {
		size_t item_size = -1;
		if (*i == 'i')
			item_size = sizeof(int);
		else if (*i == 's')
			item_size = 1; // NULL byte
		else if (*i == 'v')
			item_size = sizeof(size_t); // Read length first
		else if (*i == 'a')
			item_size = sizeof(int); // Read length first
		else {
			lerror("Invalid template item '%c'", *i);
			return -1;
		}
		
		if (read_ptr + item_size > end_ptr) {
			lerror("parse_request_structure passed end_ptr");
			return -1;
		}
		
		if (*i == 'i') {
			buff_i = *(int*)read_ptr;
			buff_i = ntohl(buff_i);
			memcpy(write_ptr, &buff_i, sizeof(int));
			
			read_ptr += item_size;
			write_ptr += item_size;
		}
		else if (*i == 's') {
			item_size = prv_safe_strdup(&buff_s, read_ptr, end_ptr);
			memcpy(write_ptr, &buff_s, sizeof(char*)); // Copy the malloced pointer
			
			read_ptr += item_size;
			write_ptr += sizeof(char*);
		}
		else if (*i == 'v') {
			/* Length is an int */
			item_size = *(int*)read_ptr;
			item_size = ntohl(item_size);
			memcpy(write_ptr, &item_size, sizeof(int));
			
			/* Move to the start of void* memory */
			read_ptr += sizeof(int);
			write_ptr += sizeof(int);
			
			if (read_ptr + item_size > end_ptr) {
				lerror("parse_request_structure passed end_ptr");
				return -1;
			}
			
			buff_v = malloc(item_size);
			memcpy(buff_v, read_ptr, item_size);
			memcpy(write_ptr, &buff_v, sizeof(void*));
			
			read_ptr += item_size;
			write_ptr += sizeof(void*);
		}
		else if (*i == 'a') {
			/* Length of array */
			int n = *(int*)read_ptr;
			n = ntohl(n);
			memcpy(write_ptr, &n, sizeof(int));
			
			read_ptr += sizeof(int);
			write_ptr += sizeof(int);
			
			/* Skip over array start and open paren */
			i += 2;
			
			char* array_end = strchr(i, ')');
			char* array_template = strndup(i, array_end - i);
			i += strlen(array_template);
			
			item_size = get_item_size(array_template);
			void* array_item = malloc(item_size);
			buff_a = vector_new(item_size, VECTOR_REMOVE | VECTOR_ORDERED);
			
			/* read each item */
			for (int j = 0; j < n; j++) {
				size_t read_size = parse_request_structure(array_item, array_template, read_ptr, end_ptr);
				vector_add(buff_a, array_item);
				read_ptr += read_size;
			}
			
			memcpy(write_ptr, &buff_a->ptr, sizeof(void*));
			write_ptr += sizeof(void*);
			
			free(buff_a); // Dont free buff_a->ptr
			item_size = 0; // We already moved everything
		}
	}
	
	return read_ptr - data;
}

size_t get_item_size(char* template) {
	size_t out = 0;
	
	for (char* i = template; *i; i++) {
		if (*i == 'a')
			out += sizeof(int) + sizeof(void*); // Length plus array ptr
		else if (*i == 's')
			out += sizeof(char*);
		else if (*i == 'v')
			out += sizeof(size_t) + sizeof(void*); // Length plus void* ptr
		else if (*i == 'i')
			out += sizeof(int);
		else {
			lerror("Invalid template item '%c'", *i);
			return -1;
		}
	}
	
	return out;
}

void free_request_structure (RequestData* to_free, char* template, void* end_ptr) {
	void* read_ptr = to_free;
	char* i;
	
	int buff_i;
	char* buff_s;
	void* buff_v;
	Vector* buff_a;
	
	
	for (i = strdup(template); *i; i++) {
		size_t item_size = -1;
		if (*i == 'i')
			item_size = sizeof(int);
		else if (*i == 's')
			item_size = sizeof(char*); // NULL byte
		else if (*i == 'v')
			item_size = sizeof(size_t) + sizeof(void*); // Read length first
		else if (*i == 'a')
			item_size = sizeof(int) + sizeof(void*); // Read length first
		else {
			lerror("Invalid template item '%c'", *i);
			return;
		}
		
		if (read_ptr + item_size > end_ptr) {
			lerror("parse_request_structure passed end_ptr");
			return;
		}
		
		if (*i == 'i')
			;// Nothing to free
		else if (*i == 's')
			free(*(char**)read_ptr);
		else if (*i == 'v') {
			/* Move to the start of void* memory */
			free(*(void**) (read_ptr + sizeof(int)));
		}
		else if (*i == 'a') {
			int n = *(int*)read_ptr;
			read_ptr += sizeof(int);
			/* Skip over open paren */
			i += 2;
			
			char* array_end = strchr(i, ')');
			char* array_template = strndup(i, array_end - i);
			i += strlen(array_template);
			
			item_size = get_item_size(array_template);
			void* array_item = malloc(item_size);
			buff_a = vector_new(item_size, VECTOR_REMOVE | VECTOR_ORDERED);
			free(buff_a->ptr);
			buff_a->ptr = *(void**)read_ptr;
			buff_a->n = n;
			
			/* read each item */
			for (int j = 0; j < n; j++) {
				void* item_free = vector_get(buff_a, j);
				free_request_structure(item_free, array_template, item_free + item_size);
			}
			
			vector_free(buff_a); // Frees *read_ptr and buff_a
			item_size = sizeof(void*);
		}
		
		read_ptr += item_size;
	}
}