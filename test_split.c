#include <check.h>

#include "picocom.h"
#include "split.h"

struct paramspec {
    char *input;
    int rc;
    int argc;
    int index;
    char *expected;
};

struct paramspec params[] = {
    {"", 0, 0, 0, NULL},                        // an empty string
    {"this is a test", 0, 4, 3, "test"},        // a string with no quotes
    {"this \"is a\" test", 0, 3, 1, "is a"},    // a string with quotes
    {"    ", 0, 0, 0, NULL},                    // all whitespace
    {"this is \\a test", 0, 4, 2, "a"},         // contains an escape
    {"this is \"a test", -1, 2, 0, NULL},       // unterminated string
};

START_TEST(test_split_quoted) {
    int argc = 0;
    char *argv[RUNCMD_ARGS_MAX + 1];
    int r;

    struct paramspec param = params[_i];

    r = split_quoted(param.input, &argc, argv, RUNCMD_ARGS_MAX);
    ck_assert(r == param.rc);
    ck_assert(argc == param.argc);

    if (param.expected != NULL) {
        ck_assert(strcmp(argv[param.index], param.expected) == 0);
    }
}

Suite * make_split_suite(void)
{
    Suite *s;
    TCase *tc_core;
    int nr_tests;

    s = suite_create("Split");

    /* Core test case */
    tc_core = tcase_create("Core");

    // count number of test cases
    nr_tests = sizeof(params)/sizeof(struct paramspec);
    tcase_add_loop_test(tc_core, test_split_quoted, 0, nr_tests);

    suite_add_tcase(s, tc_core);

    return s;
}
