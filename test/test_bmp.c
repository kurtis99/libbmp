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

static void test_color(const struct Color *const c1, const int r,
			const int g, const int b)
{
	ck_assert_int_eq(c1->red, r);
	ck_assert_int_eq(c1->green, g);
	ck_assert_int_eq(c1->blue, b);
}

START_TEST (test_bmp_BMP_get_pixel)
{
	struct BMP *b;
	struct Color c;
	b = BMP_from_file(xstr(BMP_TEST_DIR) "images/24x24.bmp");
	ck_assert(b != NULL);

	c = BMP_get_pixel(b, 0, 6);

	test_color(&c, 0xCE, 0xC9, 0x0);

	BMP_destroy(b);
}
END_TEST

START_TEST (test_bmp_ntz)
{
	int i;
	for (i = 0; i <= 32; i++)
		ck_assert_int_eq(ntz(1UL << i), i);
}
END_TEST

START_TEST (test_bmp_pop)
{
	int i;

	for (i = 0; i < 32; i++)
		ck_assert_int_eq(pop(1UL << i), 1);

	ck_assert_int_eq(pop(0x0), 0);
	ck_assert_int_eq(pop(0xFFFFFFFF), 32);
	ck_assert_int_eq(pop(0xFF), 8);
	ck_assert_int_eq(pop(0xFFFF), 16);
	ck_assert_int_eq(pop(0x1FF), 9);
	ck_assert_int_eq(pop(0x55555555), 16);
}
END_TEST

START_TEST (test_bmp_get_bitoffset)
{
	struct b_offset bo;

	bo = get_bitoffsets(0xFF000000);
	ck_assert_int_eq(bo.offset, 16);
	ck_assert_int_eq(bo.len, 8);

	bo = get_bitoffsets(0b00111100);
	ck_assert_int_eq(bo.offset, 2);
	ck_assert_int_eq(bo.len, 4);
}
END_TEST


START_TEST (test_bmp__get_bits)
{
	uint32_t bits;

	bits = _do_mask(5) | 0x0 << 5 | _do_mask(5) << 10;

	ck_assert_uint_eq(_get_bits(bits, 0, 5), _do_mask(5));
	ck_assert_uint_eq(_get_bits(bits, 5, 5), 0b0);
	ck_assert_uint_eq(_get_bits(bits, 10, 5), _do_mask(5));

	bits = _do_mask(8) | 0x0 << 8 | _do_mask(8) << 16;

	ck_assert_uint_eq(_get_bits(bits, 0, 8), _do_mask(8));
	ck_assert_uint_eq(_get_bits(bits, 8, 8), 0x0);
	ck_assert_uint_eq(_get_bits(bits, 16, 8), _do_mask(8));
}
END_TEST

START_TEST (test_bmp__do_mask)
{
	ck_assert_uint_eq(_do_mask(0), 0b0);
	ck_assert_uint_eq(_do_mask(1), 0b1);
	ck_assert_uint_eq(_do_mask(2), 0b11);
	ck_assert_uint_eq(_do_mask(3), 0b111);
	ck_assert_uint_eq(_do_mask(4), 0b1111);
	ck_assert_uint_eq(_do_mask(5), 0b11111);
	ck_assert_uint_eq(_do_mask(6), 0b111111);
	ck_assert_uint_eq(_do_mask(7), 0b1111111);
	ck_assert_uint_eq(_do_mask(8), 0b11111111);
	ck_assert_uint_eq(_do_mask(9), 0b111111111);
	ck_assert_uint_eq(_do_mask(10), 0b1111111111);
	ck_assert_uint_eq(_do_mask(11), 0b11111111111);
	ck_assert_uint_eq(_do_mask(12), 0b111111111111);

	ck_assert_uint_eq(_do_mask(16), 0xffff);
	ck_assert_uint_eq(_do_mask(24), 0xffffff);
	ck_assert_uint_eq(_do_mask(32), 0xffffffff);
}
END_TEST

START_TEST (test_bmp_get_at_offset)
{
	unsigned char test_arr[] = {0x11, 0x22, 0x33, 0x44};

	ck_assert_uint_eq(get_at_offset(test_arr, 0, 1), 0x1);
	ck_assert_uint_eq(get_at_offset(test_arr, 8, 1), 0x0);
	ck_assert_uint_eq(get_at_offset(test_arr, 16, 1), 0x1);
	ck_assert_uint_eq(get_at_offset(test_arr, 24, 1), 0x0);

	ck_assert_uint_eq(get_at_offset(test_arr, 0, 2), 0x1);
	ck_assert_uint_eq(get_at_offset(test_arr, 1, 2), 0x0);

	ck_assert_uint_eq(get_at_offset(test_arr, 0, 4), 0x1);
	ck_assert_uint_eq(get_at_offset(test_arr, 1, 4), 0x1);
	ck_assert_uint_eq(get_at_offset(test_arr, 2, 4), 0x2);
	ck_assert_uint_eq(get_at_offset(test_arr, 3, 4), 0x2);
	ck_assert_uint_eq(get_at_offset(test_arr, 4, 4), 0x3);
	ck_assert_uint_eq(get_at_offset(test_arr, 5, 4), 0x3);
	ck_assert_uint_eq(get_at_offset(test_arr, 6, 4), 0x4);
	ck_assert_uint_eq(get_at_offset(test_arr, 7, 4), 0x4);

	ck_assert_uint_eq(get_at_offset(test_arr, 0, 8), 0x11);
	ck_assert_uint_eq(get_at_offset(test_arr, 1, 8), 0x22);
	ck_assert_uint_eq(get_at_offset(test_arr, 2, 8), 0x33);
	ck_assert_uint_eq(get_at_offset(test_arr, 3, 8), 0x44);

	ck_assert_uint_eq(get_at_offset(test_arr, 0, 16), 0x1122);
	ck_assert_uint_eq(get_at_offset(test_arr, 1, 16), 0x3344);

	ck_assert_uint_eq(get_at_offset(test_arr, 0, 24), 0x112233);

	ck_assert_uint_eq(get_at_offset(test_arr, 0, 32), 0x11223344);
}
END_TEST

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
	tcase_add_test(tc_core, test_bmp_ntz);
	tcase_add_test(tc_core, test_bmp_pop);
	tcase_add_test(tc_core, test_bmp_align);
	tcase_add_test(tc_core, test_bmp__do_mask);
	tcase_add_test(tc_core, test_bmp__get_bits);
	tcase_add_test(tc_core, test_bmp_get_at_offset);

	tcase_add_test(tc_core, test_bmp_read_file);
	tcase_add_test(tc_core, test_bmp_read_file_fail);
	tcase_add_test(tc_core, test_bmp_BMP_get_pixel);

	suite_add_tcase(s, tc_core);

	return s;
}
