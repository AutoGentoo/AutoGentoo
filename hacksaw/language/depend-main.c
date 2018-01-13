#include <stdio.h>
#include <depend.h>

DependExpression* depend_parse (char* buffer);

int main(int argc, char** argv) {
    if (argc < 2) {
        fprintf (stderr, "Enter a string to parse!\n");
        return 1;
    }
    DependExpression* temp = depend_parse (argv[1]);
    debug_dependexpression(temp);
    free_dependexpression(temp);
    return 0;
}