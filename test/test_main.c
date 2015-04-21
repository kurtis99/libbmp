#include <stdlib.h>
#include "test_main.h"

Suite* bmp_suite(void);
Suite* file_io_suite(void);

int main (void)
{
	int failed;

	Suite *s;
	SRunner *sr;

	s = bmp_suite();
	sr = srunner_create(s);

	srunner_add_suite (sr, file_io_suite());

	//srunner_run_all(sr, CK_NORMAL);
	srunner_run_all(sr, CK_VERBOSE);
	failed = srunner_ntests_failed(sr);
	srunner_free(sr);
	return (failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
