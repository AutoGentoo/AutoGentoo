//
// Created by atuser on 4/23/19.
//

#ifndef AUTOGENTOO_DATABASE_H
#define AUTOGENTOO_DATABASE_H

#include <stdio.h>
#include "portage.h"
#include "package.h"

typedef struct __Database Database;

typedef enum {
	DATABASE_STOP,
	DATABASE_DEPEND,
	DATABASE_EBUILD,
} database_t;

struct __Database {
	FILE* target;
	
	Repository* repo;
};

int database_read_int(Database* db);
char* database_read_str(Database* db);
long database_read_long(Database* db);

void database_write_str(Database* db, char* str);
void database_write_int(Database* db, int i);
void database_write_long(Database* db, long l);

Dependency* database_read_dependency(Database* db);
void database_write_dependency(Database* db, Dependency* dp);

Vector* database_read_vector(Database* db);
void database_write_vector(Database* db, Vector* vec, char* template);

Map* database_read_map(Database* db);
void database_write_map(Database* db, Map* m);

void database_read(Database* db);
void database_write(Database* db);

#endif //AUTOGENTOO_DATABASE_H
