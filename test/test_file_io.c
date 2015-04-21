#include <stdlib.h>

#include "test_main.h"

START_TEST (test_bmp_read_file)
{
	struct BMP *b;

	b = BMP_from_file(xstr(BMP_TEST_DIR) "images/24x24.bmp");
	ck_assert(b != NULL);
	BMP_destroy(b);
}
END_TEST

START_TEST (test_read_missing_file)
{
	struct BMP *b;

	b = BMP_from_file("/some/foo/path/images/24x24.bmp");
	ck_assert(b == NULL);
	BMP_destroy(b);
}
END_TEST

Suite* file_io_suite(void)
{
	Suite *s;
	TCase *tc_core;

	s = suite_create("File IO capabilities, binary IO");

	/* Test to check file io capabilities */
	tc_core = tcase_create("binary");
	tcase_add_test(tc_core, test_bmp_read_file);
	tcase_add_test(tc_core, test_read_missing_file);

	suite_add_tcase(s, tc_core);

	return s;
}
