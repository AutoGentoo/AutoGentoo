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
	char* path;
	char* parent_dir;
	
	fp_node_t type;
	FPNode* next;
};

FPNode* open_directory_stat(mode_t st_mode, int buf_fd, char* parent, char* path);
FPNode* open_directory(char* path);
FPNode* open_directory_at(int parent_dir, char* parent_path, char* path);
void fpnode_free(FPNode* ptr);

#endif //AUTOGENTOO_DIRECTORY_H
