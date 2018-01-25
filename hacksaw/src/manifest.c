#include <string.h>
#include <stdlib.h>
#include <autogentoo/hacksaw/portage/package.h>

void manifest_parse (Package* pkg) {
    char manifest_file[256];
    package_get_file(pkg, manifest_file);
    
    FILE* fp = fopen (manifest_file, "r");
    char* line;
    size_t len = 0;
    ssize_t read;
    pkg->manifest->package = pkg;

    pkg->manifest->entries = vector_new(sizeof(ManifestEntry), REMOVE | UNORDERED);
    int pgp_status = 0; // 0: no, 1: message, 2: signature
    while ((read = getline(&line, &len, fp)) != -1) {
        line[strlen(line) - 1] = '\0'; // Remove the newline
        if (strncmp (line, "-----", 5) == 0) {
            if (strncmp (line, "-----BEGIN PGP SIGNED MESSAGE-----", 34) == 0) {
                pgp_status = 1;
                pkg->manifest->is_signed = 1;
            }
            else if (strncmp(line, "-----BEGIN PGP SIGNATURE-----", 29) == 0) {
                pgp_status = 2;
            }
            else if (strncmp(line, "-----END PGP SIGNATURE-----", 27) == 0) {
                pgp_status = 0;
            }
            continue;
        }

        if (pgp_status == 1) {
            if (strncmp (line, "Hash: ", 6) == 0) {
                pkg->manifest->hash.hash_type = get_hash_type(&line[6]);
                continue;
            }
        }
        else if (pgp_status == 2) {
            if (strncmp (line, "Version: ", 9) == 0 || strncmp(line, "\0", 1) == 0);
            else {
                strcat(pkg->manifest->hash.hash, line);
            }
            continue;
        }

        ManifestEntry temp;
        entry_parse(&temp, line);
        /** Dont initialize versions anymore (moved to package_new ());
        if (entry_parse(&temp, line) == EBUILD) {
            EbuildVersion v_temp;
            char* filename_temp = malloc(strlen (pkg->category->name) + strlen(temp.filename) + 1);
            filename_temp[0] = 0;
            strcat (filename_temp, pkg->category->name);
            strcat (filename_temp, "/");
            strncat (filename_temp, temp.filename, strrchr (temp.filename, '.') - temp.filename);
            
            EbuildVersion* t = &atom_parse(filename_temp)->version;
            if (error == 1) {
                printf ("%s\n", filename_temp);
                error = 0;
            }else {
                vector_add (pkg->ebuilds, t);
            }
            free (filename_temp);
        }
        */
        vector_add(pkg->manifest->entries, &temp);
    }
    fclose (fp);
}

manifest_t entry_parse (ManifestEntry* entry, char* str) {
    StringVector* tokens = string_vector_new();
    char* i;
    int j;
    for (j=0, i = strtok(str, " \n");
            i != NULL;
            i = strtok(NULL, " \n"), j++) {
        string_vector_add(tokens, i);
    }
    
    entry->type = get_entry_type(string_vector_get(tokens, 0));
    strcpy(entry->filename, string_vector_get(tokens, 1));
    sscanf(string_vector_get(tokens, 2), "%u", (unsigned int*)&entry->size);
    entry->hashes = vector_new(sizeof(HashEntry), UNORDERED | REMOVE);
    
    int n;
    for (n=3; (n + 1) < j; n += 2) {
        hash_t t = get_hash_type(string_vector_get(tokens, n));
        if (t < 0) {
            fprintf(stderr, "Hash: %s could not be found!", string_vector_get(tokens, n));
            continue;
        }
        HashEntry en;
        en.hash_type = t;
        strcpy(en.hash, string_vector_get(tokens, n+1));
        vector_add(entry->hashes, &en);
    }
    free (tokens);
    return entry->type;
}

hash_t get_hash_type (char* hash) {
    char* mapped_str[6] = {
            "MD5",
            "SHA1",
            "SHA256",
            "SHA512",
            "RMD160",
            "WHIRLPOOL"
    };

    int i;
    for (i=0; i!=sizeof(mapped_str) / sizeof(char*); i++) {
        if (strcmp(mapped_str[i], hash) == 0) {
            return (hash_t)i;
        }
    }
    (hash_t)-1;
}

manifest_t get_entry_type (char* str) {
    char* mapped_str[4] = {
            "EBUILD",
            "MISC",
            "AUX",
            "DIST"
    };

    int i;
    for (i=0; i!=sizeof(mapped_str) / sizeof(char*); i++) {
        if (strcmp(mapped_str[i], str) == 0) {
            return (manifest_t)i;
        }
    }
    (manifest_t)-1;
}