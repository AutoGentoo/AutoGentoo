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

Keyword* accept_keyword_parse(FILE* fp) {
	char* line = NULL;
	size_t n = 0;
	if (getline(&line, &n, fp) <= 0)
		return NULL;
	
	if (line[0] == '#' || line[0] == '\n' || line[0] == 0) {
		free(line);
		return accept_keyword_parse(fp);
	}
	
	
	char* atom_splt = strchr(line, ' ');
	*atom_splt = 0;
	char* atom = strdup(line);
	
	Keyword* keyword = malloc(sizeof(Keyword));
	
	keyword_parse(keyword->keywords, atom_splt + 1);
	keyword->atom = atom_parse(atom);
	free(atom);
	free(line);
	keyword->next = NULL;
	return keyword;
}

void emerge_parse_keywords(Emerge* emerge) {
	char path[256];
	sprintf(path, "%s/etc/portage/package.accept_keywords", emerge->root);
	
	struct stat st;
	if (stat(path, &st) == -1) {
		if (errno == ENOENT)
			return;
		plog_error("Failed to open %s", path);
		return;
	}
	
	struct fp_node {
		FILE* fp;
		struct fp_node* next;
	}* files = malloc(sizeof(struct fp_node));
	
	struct fp_node *current = files;
	current->fp = NULL;
	
	if (!S_ISREG(st.st_mode)) {
		int dir_fd = open(path, O_RDONLY);
		
		DIR *d;
		struct dirent *dir;
		d = opendir(path);
		if (d) {
			while ((dir = readdir(d)) != NULL) {
				
				int fd_temp = openat(dir_fd, dir->d_name, O_RDONLY);
				if (fd_temp < 0)
					continue;
				current->fp = fdopen(fd_temp, "r");
				current->next = malloc(sizeof(struct fp_node));
				current->next->fp = NULL;
				current = current->next;
				current->next = NULL;
			}
			closedir(d);
		}
	}
	else {
		files->fp = fopen(path, "r");
		files->next = NULL;
	}
	
	for (current = files; current->fp;) {
		Keyword* current_keyword = accept_keyword_parse(current->fp);
		while (current_keyword) {
			Package* target = map_get(emerge->repo->packages, current_keyword->atom->key);
			if (!target) {
				plog_warn("Package %s not found (package.accept_keywords)", current_keyword->atom->key);
				keyword_free(current_keyword);
				return;
			}
			current_keyword->next = target->keywords;
			target->keywords = current_keyword;
			current_keyword = accept_keyword_parse(current->fp);
		}
		
		struct fp_node* old = current;
		current = current->next;
		fclose(old->fp);
		free(old);
	}
}

void keyword_free(Keyword* keyword) {
	atom_free(keyword->atom);
	free(keyword);
}