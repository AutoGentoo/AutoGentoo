/*
 * hash.c
 *
 * Copyright 2017 Andrei Tumbar <atuser@Hyperion>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 *
 *
 */


#include <stdio.h>
#include <openssl/sha.h>
#include <string.h>
#include <stdlib.h>
#include <hash.h>

int hash_sha256 (unsigned char* dest, char* file_path) {
    FILE* file = fopen(file_path, "rb");
    if(!file) return -1;
    
    // Init the hash object
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    
    // Use heap not to overflow stack
    const int bufSize = 32768;
    char* buffer = malloc(bufSize);
    
    int bytesRead = 0;
    if(!buffer) return -1;
    while((bytesRead = fread (buffer, 1, bufSize, file))) {
        SHA256_Update(&sha256, buffer, bytesRead);
    }
    SHA256_Final(hash, &sha256);
    
    strcpy(dest, hash);
    fclose(file);
    free (buffer);
    
    return 0;
}

int check_hash (unsigned char* check, char* file_path) {
    unsigned char filehash[SHA256_DIGEST_LENGTH];
    if (hash_sha256 (filehash, file_path) != 0) {
        printf ("failed to check hash\n");
        return 2;
    }
    if (strncmp (check, filehash, SHA256_DIGEST_LENGTH) != 0) {
        printf ("failed to verify hash\n");
        return 1;
    }
    return 0;
}