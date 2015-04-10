#include <stdlib.h>
#include <check.h>

#include "../src/bmp.h"

#define xstr(s) str(s)
#define str(s) #s

START_TEST (test_bmp_read_file)
{
	struct BMP *b;

	b = BMP_from_file(xstr(BMP_TEST_DIR) "images/24x24.bmp");
	ck_assert(b != NULL);
	BMP_destroy(b);
}
END_TEST

START_TEST (test_bmp_read_file_fail)
{
	struct BMP *b;

	b = BMP_from_file("/some/foo/path/images/24x24.bmp");
	ck_assert(b == NULL);
	BMP_destroy(b);
}
END_TEST

Suite* bmp_master_suite(void)
{
	Suite *s;
	TCase *tc_core;

	s = suite_create("BMP general operation");

	/* Test to check file io capabilities */
	tc_core = tcase_create("Core");
	tcase_add_test(tc_core, test_bmp_read_file);
	tcase_add_test(tc_core, test_bmp_read_file_fail);

	suite_add_tcase(s, tc_core);

	return s;
}
