#include <stdlib.h>

#include "test_main.h"

/* .c file is included to test static functions */
#include "../src/bmp.c"

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

START_TEST (test_bmp_bits)
{
	int i;

	for (i = 0; i <= 32; i++)
		ck_assert_int_eq(post_zeros(1UL << i), i);
}
END_TEST

Suite* bmp_suite(void)
{
	Suite *s;
	TCase *tc_core;

	s = suite_create("BMP general operation");

	/* Test to check file io capabilities */
	tc_core = tcase_create("Core");
	tcase_add_test(tc_core, test_bmp_read_file);
	tcase_add_test(tc_core, test_bmp_read_file_fail);
	tcase_add_test(tc_core, test_bmp_bits);

	suite_add_tcase(s, tc_core);

	return s;
}
