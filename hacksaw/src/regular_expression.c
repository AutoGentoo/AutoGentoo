//
// Created by atuser on 10/21/17.
//

#include <stdio.h>

#define PCRE2_CODE_UNIT_WIDTH 8

#include <pcre2.h>
#include <string.h>
#include <regex.h>

#include <autogentoo/hacksaw/tools/string_vector.h>
#include <autogentoo/hacksaw/tools/regular_expression.h>

RegexMatch* regex_full(const char* value, char* pattern) {
	pcre2_code* re;
	pcre2_match_data* match_data;
	PCRE2_SIZE erroffset, *ovector;
	
	int errorcode;
	int rc;
	
	re = pcre2_compile((PCRE2_SPTR)pattern, -1, 0, &errorcode, &erroffset, NULL);
	if (re == NULL) {
		PCRE2_UCHAR8 buffer[120];
		(void)pcre2_get_error_message(errorcode, buffer, 120);
		fprintf(stderr, "pcre2 compile error: %s\n", buffer);
		fprintf(stderr, "%s\n", pattern);
		fprintf(stderr, "%*s^\n", (int)erroffset, "");
		exit(1);
	}
	
	RegexMatch* out = NULL;
	
	match_data = pcre2_match_data_create(20, NULL);
	
	out = malloc (sizeof (RegexMatch) * rc);
	
	for (rc = pcre2_match(re, (PCRE2_SPTR)value, -1, 0, 0, match_data, NULL);
			rc; rc = pcre2_match(re, (PCRE2_SPTR)value, -1, 0, 0, match_data, NULL)) {
		
		ovector = pcre2_get_ovector_pointer(match_data);
		printf( "Match succeeded at offset %zu\n", ovector[0] );
		/* Use ovector to get matched strings */
		int i;
		for(i = 0; i < rc; i++) {
			PCRE2_SPTR start = (PCRE2_SPTR)value + ovector[2*i];
			PCRE2_SIZE slen = ovector[2*i+1] - ovector[2*i];
			printf("%2d: %.*s\n", i, (int)slen, (char *)start);
			
			out[i].length = (size_t)slen;
			out[i].match = strndup(value + (size_t)start, slen);
			out[i].name = strdup((char*)pcre2_get_mark(match_data));
			printf("name: %s\nmatch: %s\nlength: %zu\n", out[i].name, out[i].match, out[i].length);
		}
	}
	
	
	
	pcre2_match_data_free(match_data);
	pcre2_code_free(re);
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
