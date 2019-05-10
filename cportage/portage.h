//
// Created by atuser on 4/23/19.
//

#ifndef AUTOGENTOO_PORTAGE_H
#define AUTOGENTOO_PORTAGE_H

#include <autogentoo/hacksaw/map.h>
#include <openssl/evp.h>
#include "constants.h"

typedef struct __Repository Repository;

#include "keywords.h"

struct __Repository {
	char* name;
	char* root_path;
	char* conf_dir;
	
	sha_hash hash_accept_keywords;
	sha_hash hash_package_use;
	sha_hash hash_make_conf;
	
	Map* packages;
	Map* accept_keywords;
	Map* package_use;
};


Repository* repository_new(char* name, char* root, char* conf_dir);

int portage_get_hash(sha_hash* target, char* path, const EVP_MD* algorithm);
int portage_get_hash_fd(sha_hash* target, int fd, const EVP_MD* algorithm);


#endif //AUTOGENTOO_PORTAGE_H
