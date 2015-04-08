#include <stdio.h>

#include "../src/bmp.h"

#define xstr(s) str(s)
#define str(s) #s

int main()
{
	struct BMP *b;

	b = BMP_from_file(xstr(BMP_TEST_DIR) "images/24x24.bmp");

	BMP_to_file(xstr(BMP_TEST_DIR) "images/24x24_copy.bmp", b);

	BMP_destroy(b);

	return 0;
}
