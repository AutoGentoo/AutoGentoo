#include <stdio.h>
#include "share.h"
#include <stdarg.h>

void printf_with_indent(char* format, ...) {
	va_list(args);
	int i;
	for (i = 0; i != indent; i++) {
		if (i % 4 == 0) {
			printf(" ");
			continue;
		}
		printf(" ");
	}
	va_start(args, format);
	vprintf(format, args);
}

int indent = 0;