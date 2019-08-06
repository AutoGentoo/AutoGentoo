#include <stdio.h>
#include <sys/stat.h>
#include <string.h>
#include <dirent.h>
#include "hacksaw/log.h"
#include "hacksaw/util.h"

void prv_mkdir(const char* dir) {
	DIR* _dir = opendir(dir);
	if (_dir) {
		closedir(_dir);
		return;
	}
	char tmp[256];
	char* p = NULL;
	size_t len;
	
	snprintf(tmp, sizeof(tmp), "%s", dir);
	len = strlen(tmp);
	if (tmp[len - 1] == '/')
		tmp[len - 1] = 0;
	for (p = tmp + 1; *p; p++) {
		if (*p == '/') {
			*p = 0;
			mkdir(tmp, 0777);
			*p = '/';
		}
	}
	mkdir(tmp, 0777);
}

void file_copy(char* src, char* dest) {
	FILE* fp_src = fopen(src, "r");
	if (fp_src == NULL)
		return lerror("could not open file '%s'", src);
	
	FILE* fp_dest = fopen(dest, "w+");
	if (fp_dest == NULL) {
		fclose(fp_src);
		return lerror("could not open file '%s' for writing", dest);
	}
	
	int c = fgetc(fp_src);
	while (c != EOF) {
		fputc(c, fp_dest);
		c = fgetc(fp_src);
	}
	
	fclose(fp_src);
	fclose(fp_dest);
}

int string_find(char** array, char* element, size_t n) {
	int i;
	for (i = 0; i != n; i++)
		if (strcmp(element, array[i]) == 0)
			return i;
	return -1;
}

void fix_path(char* ptr) {
	size_t n = strlen(ptr);
	int i;
	for (i = 1; i != n; i++) {
		if (ptr[i] == '/' && ptr[i - 1] == '/') {
			strcpy(&ptr[i - 1], &ptr[i]);
			ptr[n - 1] = 0;
			n--;
		}
	}
}

char* string_strip (char* str) {
	size_t len = strlen (str);
	
	int i;
	for (i = 0; i < len && str[i] == ' '; i++);
	
	int j;
	for (j = (int)len; j > i && str[j] == ' '; j--);
	
	return strndup(str + i, (size_t)j - i);
}