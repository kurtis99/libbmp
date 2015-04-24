#include <stdlib.h>
#include <limits.h>

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

#if 0
START_TEST (test_bmp_bits)
{
	int i;
	for (i = 0; i <= 32; i++)
		ck_assert_int_eq(post_zeros(1UL << i), i);
}
END_TEST
#endif

START_TEST (test_bmp_align)
{
	size_t i;

	ck_assert_int_eq(align_up(758, 16), 768);

	ck_assert_int_eq(align_up(0, 4), 4);
	ck_assert_int_eq(align_up(1, 4), 4);
	ck_assert_int_eq(align_up(2, 4), 4);
	ck_assert_int_eq(align_up(3, 4), 4);
	ck_assert_int_eq(align_up(4, 4), 4);

	ck_assert_int_eq(align_up(5, 4), 8);
	ck_assert_int_eq(align_up(8, 4), 8);

	ck_assert_int_eq(align_up(9, 4), 12);

	for (i = 1 ; i < 100; i++)
		ck_assert_int_eq(align_up(i, 1), i);

	for (i = 0 ; i < sizeof(int) * 8; i++)
		ck_assert_int_eq(align_up(1, 1 << i), 1 << i);

	/* 1 BPP */
	ck_assert_int_eq(align_up_bpp(0, 1, 16), 16);
	ck_assert_int_eq(align_up_bpp(15, 1, 16), 16);
	ck_assert_int_eq(align_up_bpp(16, 1, 16), 16);
	ck_assert_int_eq(align_up_bpp(17, 1, 16), 16);
	for (i = 0; i <= 128; i++)
		ck_assert_int_eq(align_up_bpp(i, 1, 16), 16);
	for (i = 129; i <= 256; i++)
		ck_assert_int_eq(align_up_bpp(i, 1, 16), 32);

	ck_assert_int_eq(align_up_bpp(123, 1, 16), 16);
	ck_assert_int_eq(align_up_bpp(1234567890, 1, 16), 154320992);

	/* 4 BPP */
	ck_assert_int_eq(align_up_bpp(0, 4, 4), 4);
	ck_assert_int_eq(align_up_bpp(1, 4, 4), 4);
	ck_assert_int_eq(align_up_bpp(4, 4, 4), 4);
	ck_assert_int_eq(align_up_bpp(5, 4, 4), 4);
	ck_assert_int_eq(align_up_bpp(1000, 4, 4), 500);
	ck_assert_int_eq(align_up_bpp(1001, 4, 4), 504);
	ck_assert_int_eq(align_up_bpp(16, 4, 4), 8);

	/* 8 BPP */
	ck_assert_int_eq(align_up_bpp(10, 8, 4), 12);
	ck_assert_int_eq(align_up_bpp(758, 8, 16), 768);
	ck_assert_int_eq(align_up_bpp(1, 8, 2), 2);

	/* 16 BPP */
	ck_assert_int_eq(align_up_bpp(0, 16, 4), 4);
	ck_assert_int_eq(align_up_bpp(1, 16, 4), 4);
	ck_assert_int_eq(align_up_bpp(2, 16, 4), 4);
	ck_assert_int_eq(align_up_bpp(3, 16, 4), 8);
	ck_assert_int_eq(align_up_bpp(4, 16, 4), 8);
	ck_assert_int_eq(align_up_bpp(5, 16, 4), 12);
	ck_assert_int_eq(align_up_bpp(6, 16, 4), 12);
	ck_assert_int_eq(align_up_bpp(7, 16, 4), 16);
	ck_assert_int_eq(align_up_bpp(8, 16, 4), 16);

	ck_assert_int_eq(align_up_bpp(2, 16, 4), 4);
	ck_assert_int_eq(align_up_bpp(1000, 16, 4), 2000);

	/* 24 BPP */
	ck_assert_int_eq(align_up_bpp(0, 24, 4), 4);
	ck_assert_int_eq(align_up_bpp(1, 24, 4), 4);
	ck_assert_int_eq(align_up_bpp(2, 24, 4), 8);
	ck_assert_int_eq(align_up_bpp(3, 24, 4), 12);
	ck_assert_int_eq(align_up_bpp(4, 24, 4), 12);
	ck_assert_int_eq(align_up_bpp(5, 24, 4), 16);

	/* 32 BPP */
	ck_assert_int_eq(align_up_bpp(0, 32, 4), 4);
	ck_assert_int_eq(align_up_bpp(1, 32, 4), 4);
	ck_assert_int_eq(align_up_bpp(2, 32, 4), 8);
	ck_assert_int_eq(align_up_bpp(3, 32, 4), 12);
	ck_assert_int_eq(align_up_bpp(4, 32, 4), 16);
	ck_assert_int_eq(align_up_bpp(5, 32, 4), 20);
	ck_assert_int_eq(align_up_bpp(6, 32, 4), 24);
	ck_assert_int_eq(align_up_bpp(7, 32, 4), 28);
	ck_assert_int_eq(align_up_bpp(8, 32, 4), 32);
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
	//tcase_add_test(tc_core, test_bmp_bits);
	tcase_add_test(tc_core, test_bmp_align);

	suite_add_tcase(s, tc_core);

	return s;
}
