#include <stdio.h>
#include <stdlib.h>

#include "bmp.h"

int main (int argc, char *argv[])
{
	struct BMP *b;
	struct Color c;

	if (argc != 2) {
		fprintf(stderr, "Need path to BMP file to dump\n");
		return EXIT_FAILURE;
	}

	b = BMP_from_file(argv[argc - 1]);
	BMP_dump(b);

	c = BMP_get_pixel(b, 0, 0);
	BMP_dump_color(&c);

	BMP_destroy(b);

	return 0;
}
