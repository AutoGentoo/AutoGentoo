//
// Created by atuser on 5/5/19.
//

#include <share.h>
#include "keywords.h"
#include "portage_log.h"
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include <dirent.h>
#include <fcntl.h>
#include <autogentoo/hacksaw/map.h>
#include "portage.h"
#include "directory.h"
#include <unistd.h>

arch_t get_arch(char* search) {
	if (!search)
		return ARCH_END;
	for (int i = 0; i < ARCH_END; i++)
		if (strcmp(keyword_links[i].str, search) == 0)
			return keyword_links[i].l;
	return ARCH_END;
}

void keyword_parse(keyword_t* out, char* line) {
	for (int i = 0; i < ARCH_END; i++)
		out[i] = KEYWORD_STABLE;
	
	line = strdup(line);
	
	char* tok;
	for (tok = strtok(line, " \n\t"); tok; tok = strtok(NULL, " \n\t")) {
		if (tok[0] == 0)
			continue;
		keyword_t opt = KEYWORD_STABLE;
		size_t offset = 0;
		if (tok[0] == '~') {
			opt = KEYWORD_UNSTABLE;
			offset = 1;
		} else if (tok[0] == '-') {
			opt = KEYWORD_BROKEN;
			offset = 1;
		}
		
		char* target = tok + offset;
		if (strcmp(target, "**") == 0) {
			for (int i = 0; i < ARCH_END; i++)
				out[i] = opt;
			continue;
		}
		if (strcmp(target, "*") == 0) {
			for (int i = 0; i < ARCH_END; i++)
				out[i] = opt;
			continue;
		}
		out[get_arch(target)] = opt;
	}
	
	free(line);
}

Keyword* accept_keyword_parse(FILE* fp, Keyword** last) {
	char* line = NULL;
	size_t n = 0;
	size_t read_size = 0;
	
	Keyword* out = NULL;
	Keyword* temp = NULL;
	
	while ((read_size = getline(&line, &n, fp)) != -1) {
		if (line[0] == '#' || line[0] == '\n' || line[0] == 0)
			continue;
		char* atom_splt = strchr(line, ' ');
		*atom_splt = 0;
		char* atom = strdup(line);
		
		temp = malloc(sizeof(Keyword));
		keyword_parse(temp->keywords, atom_splt + 1);
		temp->atom = atom_parse(atom);
		free(atom);
		
		if (!out)
			*last = temp;
		
		temp->next = out;
		out = temp;
	}
	free(line);
	
	return out;
}

void emerge_parse_keywords(Emerge* emerge) {
	char path[256];
	sprintf(path, "%s/etc/portage/package.accept_keywords", emerge->root);
	
	FPNode* files = open_directory(path);
	FPNode* old;
	
	Keyword* parsed = NULL;
	Keyword* temp = NULL;
	
	for (FPNode* current = files; current;) {
		if (current->type == FP_NODE_DIR) {
			old = current;
			free(old->filename);
			free(old->parent_dir);
			free(old->path);
			current = current->next;
			free(old);
			continue;
		}
		
		FILE* fp = fopen(current->path, "r");
		Keyword* last;
		temp = accept_keyword_parse(fp, &last);
		
		last->next = parsed;
		parsed = temp;
		fclose(fp);
		
		old = current;
		free(old->filename);
		free(old->parent_dir);
		free(old->path);
		current = current->next;
		free(old);
	}
	
	Keyword* next;
	Keyword* current = parsed;
	while (current) {
		next = current->next;
		Package* target = map_get(emerge->repo->packages, current->atom->key);
		if (!target) {
			plog_warn("Package %s not found (package.accept_keywords)", current->atom->key);
			keyword_free(current);
			return;
		}
		current->next = target->keywords;
		target->keywords = current;
		current = next;
	}
}

void keyword_free(Keyword* keyword) {
	atom_free(keyword->atom);
	free(keyword);
}