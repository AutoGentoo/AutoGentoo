//
// Created by atuser on 5/5/19.
//

#include <share.h>
#include "keywords.h"
#include "portage_log.h"
#include <string.h>
#include <autogentoo/hacksaw/map.h>
#include "portage.h"
#include "directory.h"

arch_t get_arch(char* search) {
	if (!search)
		return ARCH_END;
	for (int i = 0; i < ARCH_END; i++)
		if (strcmp(keyword_links[i].str, search) == 0)
			return keyword_links[i].l;
	return ARCH_END;
}

void keyword_parse(keyword_t* out, char* line) {
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

void accept_keyword_parse(FILE* fp, Vector* keywords) {
	char* line = NULL;
	size_t n = 0;
	size_t read_size = 0;
	
	while ((read_size = getline(&line, &n, fp)) != -1) {
		if (line[0] == '#' || line[0] == '\n' || line[0] == 0)
			continue;
		line[read_size - 1] = 0;
		char* atom_splt = strchr(line, ' ');
		*atom_splt = 0;
		char* atom = strdup(line);
		
		Keyword* new_keyword = malloc(sizeof(Keyword));
		keyword_parse(new_keyword->keywords, atom_splt + 1);
		new_keyword->atom = atom_parse(atom);
		free(atom);
		
		vector_add(keywords, new_keyword);
	}
	
	free(line);
}

void emerge_parse_keywords(Emerge* emerge) {
	char path[256];
	sprintf(path, "%setc/portage/package.accept_keywords", emerge->root);
	
	FPNode* files = open_directory(path);
	
	/* REMOVE WHEN PROFILE IS READY */
	Vector* keywords = vector_new(VECTOR_REMOVE | VECTOR_ORDERED);
	
	for (FPNode* current = files; current; current = current->next) {
		if (current->type == FP_NODE_DIR)
			continue;
		
		FILE* fp = fopen(current->path, "r");
		if (!fp) {
			plog_error("Failed to open %s", current->path);
			fpnode_free(current);
			return;
		}
		
		accept_keyword_parse(fp, keywords);
		fclose(fp);
	}
	
	fpnode_free(files);
	
	for (int i = 0; i < keywords->n; i++) {
		Keyword* current = vector_get(keywords, i);
		
		for (Repository* repo = emerge->repos; repo; repo = repo->next) {
			if (current->atom->repo_selected == ATOM_REPO_DEFINED
			    && strcmp(current->atom->repository, repo->name) != 0)
				continue;
			
			Package* target = map_get(repo->packages, current->atom->key);
			
			if (!target) {
				continue;
			}
			
			Keyword* new = malloc(sizeof(Keyword));
			new->next = target->keywords;
			new->atom = atom_dup(current->atom);
			memcpy(new->keywords, current->keywords, sizeof(keyword_t) * ARCH_END);
			
			new->next = target->keywords;
			target->keywords = new;
		}
	}
	
	for (int i = 0; i < keywords->n; i++)
		keyword_free(vector_get(keywords, i));
	
	vector_free(keywords);
}

void keyword_free(Keyword* keyword) {
	atom_free(keyword->atom);
	free(keyword);
}