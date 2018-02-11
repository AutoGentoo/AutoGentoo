//
// Created by atuser on 10/21/17.
//

#include <stdio.h>

#define PCRE2_CODE_UNIT_WIDTH 8

#include <pcre2.h>
#include <string.h>
#include <regex.h>

#include <autogentoo/hacksaw/tools/string_vector.h>

Vector* regex_full(const char* value, char* pattern) {
	pcre2_code* re;
	pcre2_match_data* match_data;
	PCRE2_SIZE erroffset, * ovector;
	int errorcode;
	int rc;
	
	re = pcre2_compile((PCRE2_SPTR8) pattern, (size_t) -1, 0, &errorcode, &erroffset, NULL);
	if (re == NULL) {
		PCRE2_UCHAR8 buffer[128];
		(void) pcre2_get_error_message(errorcode, buffer, 128);
		return NULL;
	}
	match_data = pcre2_match_data_create(20, NULL);
	PCRE2_SPTR8 cursor = (PCRE2_SPTR8) value;
	Vector* out = vector_new(sizeof(StringVector*), REMOVE | UNORDERED);
	while (1) {
		rc = pcre2_match(re, cursor, (size_t) -1, 0, 0, match_data, NULL);
		if (rc <= 0) {
			break;
		}
		StringVector* entry = string_vector_new();
		ovector = pcre2_get_ovector_pointer(match_data);
		int i;
		for (i = 1; i < rc; i++) {
			char* substring_start = (char*) cursor + ovector[2 * i];
			size_t substring_length = ovector[2 * i + 1] - ovector[2 * i];
			char copy[substring_length + 1];
			strncpy(copy, substring_start, substring_length);
			copy[substring_length] = 0;
			if (substring_length != 0) {
				string_vector_add(entry, copy);
			}
		}
		cursor += ovector[1];
		vector_add(out, &entry);
	}
	pcre2_match_data_free(match_data);
	pcre2_code_free(re);
	return out;
}

int regex_simple(char* dest, char* string, char* pattern) {
	regex_t regexCompiled;
	regmatch_t groupArray[2];
	
	if (regcomp(&regexCompiled, pattern, REG_EXTENDED)) {
		printf("Could not compile regular expression.\n");
		return 0;
	};
	int found = 0;
	if (regexec(&regexCompiled, string, 2, groupArray, 0) == 0) {
		unsigned int g;
		for (g = 1; g <= 1; g++) {
			if (groupArray[g].rm_so == (size_t) -1) {
				break;  // No more groups
			}
			found = 1;
			char sourceCopy[strlen(string) + 1];
			strcpy(sourceCopy, string);
			sourceCopy[groupArray[g].rm_eo] = 0;
			strcpy(dest, sourceCopy + groupArray[g].rm_so);
		}
	}
	
	if (!found) {
		strcpy(dest, "");
		return 0;
	}
	
	regfree(&regexCompiled);
	return 1;
}
