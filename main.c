#include <stdio.h>
#include <tools/vector.h>
#include <tools/string_vector.h>
#include <tools/regular_expression.h>
#include <tools/config.h>
#include <test/debug.h>

void print_bin (void* ptr, int n, size_t size) {
    int i;
    for (i=0; i != (size * n); i++) {
        int j;
        for (j=0; j<8; j++) {
            printf("%d",  !!((((char*)ptr)[i] << j) & 0x80));
        }

        if (!((i + 1) % size)) {
            printf (" ");
        }
    }
    printf("\n");
}

void print_vec(Vector* vec) {
    print_bin(vec->ptr, vec->n, vec->size);
    fflush(stdout);
}

void print_string_vec(StringVector* vec) {
    int i;
    for (i=0; i!=vec->n; i++) {
        printf("%p (%s) ", string_vector_get(vec, i), string_vector_get(vec, i));
    }
    printf("\n");
}

struct __test1 {
    int a;
    int b;
};

struct __test2 {
    int a;
    int b;
    int c;
};

int main() {
    printf ("Test Vector");
    Vector* testVec = vector_new(sizeof(int), UNORDERED | REMOVE);
    print_vec(testVec);

    int el_one = 0x0;
    int el_two = 0xAAAAAAAA;

    vector_add(testVec, &el_one);
    vector_add(testVec, &el_two);
    printf("%d\n", testVec->n);
    print_vec(testVec);

    int el_1_5 = 0xFFFFFFFF;
    vector_insert(testVec, &el_1_5, 1);
    print_vec(testVec);

    vector_remove(testVec, 1);
    print_vec(testVec);
    vector_add(testVec, &el_two);
    print_vec(testVec);

    vector_free(testVec);

    printf ("Test StringVector\n");
    StringVector* strvec = string_vector_new();
    char n1[] = "NewString1";
    string_vector_add(strvec, n1);
    print_string_vec(strvec);
    char n2[] = "NewString2";
    string_vector_add(strvec, n2);
    print_string_vec(strvec);
    string_vector_remove (strvec, 0);
    print_string_vec(strvec);

    string_vector_insert (strvec, "TestInsert", 0);
    print_string_vec(strvec);

    string_vector_free (strvec);

    printf ("Test regex\n");
    StringVector* testregex = string_vector_new();
    print_string_vec(testregex);


    Config* test_config = config_read("/etc/portage/make.conf");
    printf("CFLAGS = %s\n", config_get(test_config, NULL, "CFLAGS"));
    printf("PORTDIR = %s\n", config_get(test_config, NULL, "PORTDIR"));
    config_free(test_config);

    Config* test_repo = config_read("/etc/portage/repos.conf/rrr");


    return 0;
}