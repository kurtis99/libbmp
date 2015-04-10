#include <stdlib.h>
#include <check.h>

Suite* bmp_master_suite(void);

int main (void)
{
	int failed;

	Suite *s;
	SRunner *sr;

	s = bmp_master_suite();
	sr = srunner_create(s);

	//srunner_run_all(sr, CK_NORMAL);
	srunner_run_all(sr, CK_VERBOSE);
	failed = srunner_ntests_failed(sr);
	srunner_free(sr);
	return (failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
