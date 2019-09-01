//
// Created by atuser on 8/28/19.
//

#include <stdint.h>
#include <string.h>
#include <autogentoo/endian_convert.h>
#include <stdlib.h>
#include <hacksaw/log.h>

int rs_read_int(void** src) {
	uint32_t len = 0;
	memcpy(*src, &len, sizeof(len));
	
	*src += sizeof(len);
	
	return ntohl(len);
}

char* rs_read_string(void** src) {
	int len = rs_read_int(src);
	
	char* out = malloc(len + 1);
	strncpy(out, *src, len);
	
	*src += len;
	
	return out;
}

char* request_structure_template_parse(char* c_template, size_t* skip) {
	char* c_template_end = c_template;
	
	int current_level = 1;
	
	if (*c_template != '(') {
		lerror("Incorrect c_template given");
		exit(1);
	}
	
	c_template_end++;
	*skip = 1;
	
	for (; *c_template_end; c_template_end++) {
		if (*c_template_end == '(')
			current_level++;
		else if (*c_template_end == ')')
			current_level--;
		
		(*skip)++;
		
		if (current_level == 0)
			break;
	}
	
	return strndup(c_template, c_template_end - c_template);
}

size_t request_structure_get_size(char* template) {
	size_t n = 0;
	
	for (char* c = template; *c; c++) {
		if (*c == 'i')
			n += sizeof(int);
		else if (*c == 's')
			n += sizeof(char*);
		else if (*c == 'a') {
			c++;
			
			size_t skip = 0;
			char* sub_template = request_structure_template_parse(c, &skip);
			
			c += skip;
			free(sub_template);
			
			/* int for length, void* for array */
			n += sizeof(int);
			n += sizeof(void*);
		}
		else {
			lerror("Invalid request_structure type '%c'", *c);
			exit(1);
		}
	}
	
	return n;
}

void* request_structure_read(void* src, char* template, size_t* skip) {
	size_t n = request_structure_get_size(template);
	void* dest = malloc(n);
	void* current = dest;
	
	void* temp_src = src;
	
	for (char* c = template; *c; c++) {
		if (*c == 'i') {
			*((int*) current) = rs_read_int(&temp_src);
			current += sizeof(int);
		}
		else if (*c == 's') {
			*((char**) current) = rs_read_string(&temp_src);
			current += sizeof(char*);
		}
		else if (*c == 'a') {
			c++;
			
			/* Get the sub-template for the array */
			size_t temp_skip = 0;
			char* sub_template = request_structure_template_parse(c, &temp_skip);
			
			/* Get the number items to read */
			int i_n = *((int*) current) = rs_read_int(&temp_src);
			current += sizeof(int);
			
			/* Create an array of request_structures, i_n long */
			void** array = malloc(sizeof(void*) * i_n);
			size_t sub_skip = 0;
			
			/* Do a request_structure_read i_n number of times */
			for (int i = 0; i < i_n; i++) {
				array[i] = request_structure_read(temp_src, sub_template, &sub_skip);
				temp_src += sub_skip;
			}
			
			/* Copy the array into the parent request_structure */
			*((void***) current) = array;
			current += sizeof(void*);
			
			/* Free the sub template and skip over the inner
			 * contents of the template we just read through */
			free(sub_template);
			c += temp_skip;
		}
		else {
			lerror("Invalid request_structure type '%c'", *c);
			exit(1);
		}
	}
	
	/* Get the source read size by substracting current read position from start */
	*skip = (size_t)(temp_src - src);
	
	return dest;
}

void request_structure_free(void* rs, char* template) {
	void* current = rs;
	
	for (char* c = template; *c; c++) {
		if (*c == 'i') {
			current += sizeof(int);
		}
		else if (*c == 's') {
			free(*((char**) current));
			current += sizeof(char*);
		}
		else if (*c == 'a') {
			c++;
			
			size_t skip = 0;
			char* sub_template = request_structure_template_parse(c, &skip);
			
			int iter_num = *(int*)current;
			current += sizeof(int);
			
			void** array = *(void***)current;
			for (int i = 0; i < iter_num; i++)
				request_structure_free(array[i], sub_template);
			
			free(array);
			
			current += sizeof(void**);
			free(sub_template);
			c += skip;
		}
		else {
			lerror("Invalid request_structure type '%c'", *c);
			exit(1);
		}
	}
	
	free(rs);
}