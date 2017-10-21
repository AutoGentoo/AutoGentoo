#include <stdio.h>
#include <tools/vector.h>

void print_bin (void* ptr, size_t size) {
    int i;
    for (i=size-1; i>=0; i--) {
        int j;
        for (j=0; j<8; j++) {
            printf("%d",  !!((((char*)ptr)[i] << j) & 0x80));
        }

        if (i % 2) {
            printf (" ");
        }
        else{
            printf("|");
        }
    }
    printf("\n");
}

void print_vec(Vector* vec) {
    print_bin(vec->ptr, (vec->n + 1) * vec->size);
    fflush(stdout);
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
    Vector* testVec = vector_new(sizeof(int), UNORDERED | KEEP);
    print_vec(testVec);

    int el_one = 0x81;
    int el_two = 0xFF;

    vector_add(testVec, &el_one);
    vector_add(testVec, &el_two);
    print_vec(testVec);
    vector_free(testVec);

    return 0;
}