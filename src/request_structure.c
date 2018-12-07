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

int prv_handle_item (void* dest, void* data, char type, size_t item_size, void* end) {
	if (data + item_size > end)
		return -1;
	
	if (type == 'v') {
		item_size = *(size_t*)dest = *(size_t*)data;
		dest += sizeof (size_t);
		data += sizeof (size_t);
		
		if (data + item_size > end)
			return -1;
		
		void* ptr = malloc (item_size);
		memcpy(ptr, data, item_size);
		*(void**)dest = ptr;
		item_size += sizeof (size_t); // Make sure we skip over size_t
	}
	else if (type == 's')
		item_size = prv_safe_strdup((char**)dest, data, end);
	else
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
	
	char* i = strdup(template);
	char* orig = i;
	
	for (; *i != 0; i++) {
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
				ptr += get_ssizeof(i);
				data += len;
				
				if (len == -1) {
					for (--j; j >= 0; j--)
						free_request_structure(((void**)ptr)[j], i, NULL);
					free (ptr);
					free_request_structure (out, template, i);
					free (orig);
					return -1;
				}
			}
			
			i = i_next + 1;
			continue;
		}
		
		size_t item_size = get_sizeof(*i);
		len = prv_handle_item (ptr, data, *i, item_size, end);
		
		if (len == -1) {
			free(orig);
			return -1;
		}
		
		ptr += item_size;
		data += len;
	}
	
	free (orig);
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
			{'v', sizeof(size_t)}, /* Use size_t because the data size is checked dynamically */
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
	char* i = strdup(template);
	size_t dif = end - template;
	char* orig = i;
	
	size_t offset = 0;
	for (; *i != 0; i++) {
		if (end && i - orig >= dif)
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
		if (*i == 'v')
			free (to_free + offset + sizeof (size_t));
		
		size_t item_size = get_sizeof(*i);
		
		if (*i == 's')
			free (*(char**)((void*)to_free + offset));
		
		offset += item_size;
	}
	
	free (orig);
}