#include <stdlib.h>
#include <check.h>

extern Suite *make_configfile_suite();
extern Suite *make_split_suite();

Suite *make_main_suite(void)
{
    Suite *s;
    s = suite_create("Main");
    return s;
}

int main(void)
{
    int number_failed;
    SRunner *sr;

    sr = srunner_create(make_main_suite());
    srunner_set_tap(sr, "test_picocom.tap");
    srunner_set_log(sr, "test_picocom.log");
    srunner_add_suite(sr, make_configfile_suite());
    srunner_add_suite(sr, make_split_suite());

    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
