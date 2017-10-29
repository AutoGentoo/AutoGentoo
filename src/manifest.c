#include <package/manifest.h>
#include <string.h>
#include <stdlib.h>
#include <tools/string_vector.h>

void manifest_parse (Manifest* manifest, FILE* fp) {
    char* line;
    size_t len = 0;
    ssize_t read;

    manifest->entries = vector_new(sizeof(ManifestEntry), REMOVE | UNORDERED);
    int pgp_status = 0; // 0: no, 1: message, 2: signature
    while ((read = getline(&line, &len, fp))) {
        line[strlen(line) - 1] = '\0'; // Remove the newline
        if (strncmp (line, "-----", 5) == 0) {
            if (strncmp (line, "-----BEGIN PGP SIGNED MESSAGE-----", 34) == 0) {
                pgp_status = 1;
                manifest->is_signed = 1;
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
                manifest->hash.hash_type = get_hash_type(&line[6]);
                continue;
            }
        }
        else if (pgp_status == 2) {
            if (strncmp (line, "Version: ", 9) == 0 || strncmp(line, "\0", 1) == 0);
            else {
                strcat(manifest->hash.hash, line);
            }
            continue;
        }

        ManifestEntry temp;
        entry_parse(&temp, line);
        vector_add(manifest->entries, &temp);
    }
}

void entry_parse (ManifestEntry* entry, char* str) {
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
    printf ("n = ");
    for (n=3; (n + 1) < j; n += 2) {
        printf ("%d, ", n);
        fflush(stdout);
        hash_t t = get_hash_type(string_vector_get(tokens, n));
        if (t < 0) {
            fprintf(stderr, "Hash: %s could not be found!", string_vector_get(tokens, n));
            break;
        }
        HashEntry en;
        en.hash_type = t;
        strcpy(en.hash, string_vector_get(tokens, n+1));
        vector_add(entry->hashes, &en);
    }
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