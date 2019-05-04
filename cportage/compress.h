//
// Created by atuser on 4/28/19.
//

#ifndef AUTOGENTOO_COMPRESS_H
#define AUTOGENTOO_COMPRESS_H

#include <stdio.h>

FILE* fread_archive(char* path, int dirfd, char* verify_sha);

#endif //AUTOGENTOO_COMPRESS_H
