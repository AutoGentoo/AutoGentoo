//
// Created by atuser on 10/21/17.
//

#include <stdio.h>
#include <regex.h>
#include <tools/regular_expression.h>
#include <string.h>

int re_match(char *string, char *pattern)
{
    int status;
    regex_t re;

    if (regcomp(&re, pattern, REG_EXTENDED|REG_NOSUB) != 0) {
        return 0;
    }

    status = regexec(&re, string, (size_t) 0, NULL, 0);
    regfree(&re);

    return !status;
}

void re_group_match (StringVector* vec, char* string, char* pattern, int max_groups) {
    regex_t regexCompiled;
    regmatch_t groupArray[max_groups+1];

    if (regcomp(&regexCompiled, pattern, REG_EXTENDED))
    {
        printf("Could not compile regular expression.\n");
        return;
    };

    if (regexec(&regexCompiled, string, max_groups+1, groupArray, 0) == 0)
    {
        unsigned int g = 0;
        for (g = 1; g <= max_groups; g++)
        {
            if (groupArray[g].rm_so == (size_t)-1)
                break;  // No more groups

            char sourceCopy[strlen(string) + 1];
            strcpy(sourceCopy, string);
            sourceCopy[groupArray[g].rm_eo] = 0;
            string_vector_add(vec, sourceCopy + groupArray[g].rm_so);
        }
    }

    regfree(&regexCompiled);
}

void re_group_get (char* dest, char* string, char* pattern) {
    int max_groups = 1;
    regex_t regexCompiled;
    regmatch_t groupArray[max_groups+1];

    if (regcomp(&regexCompiled, pattern, REG_EXTENDED))
    {
        printf("Could not compile regular expression.\n");
        return;
    };
    int found = 0;
    if (regexec(&regexCompiled, string, max_groups+1, groupArray, 0) == 0)
    {
        unsigned int g;
        for (g = 1; g <= max_groups; g++)
        {
            if (groupArray[g].rm_so == (size_t)-1) {
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
    }

    regfree(&regexCompiled);
}