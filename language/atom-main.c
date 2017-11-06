#include <stdio.h>
#include <package.h>

PackageSelector* package_parse (char* buffer);

int main(int argc, char** argv) {
    if (argc < 2) {
        fprintf (stderr, "Enter a string to parse!\n");
        return 1;
    }
    PackageSelector* temp = package_parse (argv[1]);
    print_package_selector(temp);
    free_package_selector(temp);
    return 0;
}