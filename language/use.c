#include <use.h>

int main(void) {
    int i;
    for (i=0; i<26; i++) sym[i] = 0;
    yyparse();
    return 0;
}