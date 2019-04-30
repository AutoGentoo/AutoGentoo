#include <stdio.h>
#include <share.h>

int main(int argc, char** argv) {
	if (argc < 2) {
		fprintf(stderr, "Enter a string to parse!\n");
		return 1;
	}
	P_Atom* temp = atom_parse(argv[1]);
	free_atom(temp);
	return 0;
}