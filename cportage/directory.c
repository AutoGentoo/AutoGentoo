//
// Created by atuser on 5/12/19.
//

#define _GNU_SOURCE

#include "directory.h"
#include <errno.h>
#include "portage_log.h"
#include <unistd.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <fcntl.h>
#include <dirent.h>
#include <string.h>

FPNode* open_directory_stat(mode_t st_mode, int buf_fd, char* parent, char* path) {
	FPNode* files = NULL;
	FPNode* temp;
	if (S_ISDIR(st_mode)) {
		DIR *d;
		struct dirent *dir;
		d = fdopendir(buf_fd);
		if (d) {
			while ((dir = readdir(d)) != NULL) {
				if (dir->d_name[0] == '.')
					continue;
				temp = malloc(sizeof(FPNode));
				temp->filename = strdup(dir->d_name);
				temp->parent_dir = strdup(parent);
				if (path)
					asprintf(&temp->path, "%s/%s/%s", parent, path, dir->d_name);
				else
					asprintf(&temp->path, "%s/%s", parent, dir->d_name);
				struct stat c_st;
				stat(temp->path, &c_st);
				
				if (S_ISREG(c_st.st_mode))
					temp->type = FP_NODE_FILE;
				else if (S_ISDIR(c_st.st_mode))
					temp->type = FP_NODE_DIR;
				
				temp->next = files;
				files = temp;
			}
			closedir(d);
		}
	}
	else if (S_ISREG(st_mode)) {
		files = malloc(sizeof(FPNode));
		files->filename = strdup(parent);
		files->type = FP_NODE_FILE;
		files->next = NULL;
	}
	
	return files;
}

FPNode* open_directory(char* path) {
	struct stat st;
	if (stat(path, &st) == -1) {
		if (errno == ENOENT)
			return NULL;
		plog_error("Failed to open %s", path);
		return NULL;
	}
	
	int buf_fd = open(path, O_RDONLY);
	return open_directory_stat(st.st_mode, buf_fd, path, NULL);
}

FPNode* open_directory_at(int parent_dir, char* parent_path, char* path) {
	if (parent_dir < 0)
		return open_directory(path);
	
	struct stat st;
	if (fstatat(parent_dir, path, &st, 0) == -1) {
		plog_error("Failed to open %s", path);
		return NULL;
	}
	
	int buf_fd = openat(parent_dir, path, O_RDONLY);
	FPNode* out = open_directory_stat(st.st_mode, buf_fd, parent_path, path);
	close(buf_fd);
	return out;
}