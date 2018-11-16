#include <stdio.h>
#include <share.h>

int main(int argc, char** argv) {
	if (argc < 2) {
		fprintf(stderr, "Enter a string to parse!\n");
		return 1;
	}
	AtomSelector* temp = atom_parse(argv[1]);
	print_atom_selector(temp);
	free_atom_selector(temp);
	return 0;
}