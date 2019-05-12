//
// Created by atuser on 5/12/19.
//

#ifndef AUTOGENTOO_DIRECTORY_H
#define AUTOGENTOO_DIRECTORY_H

#include <stdio.h>
#include <fcntl.h>

typedef struct fp_node FPNode;

typedef enum {
	FP_NODE_FILE,
	FP_NODE_DIR
} fp_node_t;

struct fp_node {
	char* filename;
	
	fp_node_t type;
	int dirfd; // null if normal file
	FILE* fp; // null if directory
	FPNode* next;
};

FPNode* open_directory_stat(mode_t st_mode, int buf_fd, char* path);
FPNode* open_directory(char* path);
FPNode* open_directory_at(int parent_dir, char* path);

#endif //AUTOGENTOO_DIRECTORY_H
