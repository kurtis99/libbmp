//#include <stdio.h>
#include <stdlib.h>
#include <check.h>

#include "../src/bmp.h"

//#define xstr(s) str(s)
//#define str(s) #s

START_TEST (test_bmp)
{
	// TODO
	ck_assert_int_eq(4, 5);
}
END_TEST

Suite* bmp_suite(void)
{
	Suite *s;
	TCase *tc_core;

	s = suite_create("BMP");

	tc_core = tcase_create("Core");

	tcase_add_test(tc_core, test_bmp);
	suite_add_tcase(s, tc_core);

	return s;
}

//int main (int argc, char *argv[])
int main ()
{
	int failed;

	Suite *s;
	SRunner *sr;

	s = bmp_suite();
	sr = srunner_create(s);

	srunner_run_all(sr, CK_NORMAL);
	failed = srunner_ntests_failed(sr);

	srunner_free(sr);

	return (failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}


#if 0
int main()
{
	struct BMP *b;

	b = BMP_from_file(xstr(BMP_TEST_DIR) "images/24x24.bmp");
	BMP_to_file(xstr(BMP_TEST_DIR) "images/24x24_copy.bmp", b);
	BMP_dump(b);
	BMP_destroy(b);

	b = BMP_from_file(xstr(BMP_TEST_DIR) "images/24x24_copy.bmp");
	BMP_dump(b);
	BMP_destroy(b);

	return 0;
}
#endif
