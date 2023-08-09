#include <check.h>

#include "configfile.h"

char *valid_config = "port = dummy\n"
"baud = 57600\n"
"echo = true\n"
"omap = {crcrlf, delbs}\n"
"initstring = \"ati4\\n\"\n";

extern int parse_args(int argc, char *argv[]);
extern void init_defaults(void);

int parse_config_buf(const char *buf) { return cfg_parse_buf(cfg, buf); }

static void setup() {
    init_defaults();
    init_config();
}

static void teardown() {}

START_TEST(test_no_error_on_missing_config) {
    ck_assert(parse_config_file("does-not-exist") == 0);
}

START_TEST(test_error_on_invalid_option) {
    ck_assert(parse_config_buf("invalid-setting = invalid-value") != 0);
}

START_TEST(test_error_on_invalid_value) {
    // pass a string to an integer option
    ck_assert(parse_config_buf("baud = badvalue") != 0);

    // pass invalid string to enum option
    ck_assert(parse_config_buf("dtr = badvalue") != 0);

    // pass invalid string to boolean option
    ck_assert(parse_config_buf("noreset = badvalue") != 0);

    // pass string to list option
    ck_assert(parse_config_buf("omap = badvalue") != 0);
}

START_TEST(test_no_error_on_valid_config) {
    ck_assert(parse_config_buf(valid_config) == 0);
}

START_TEST(test_setting_config_sets_option) {
    char *args[] = {"picocom"};

    ck_assert(parse_config_buf(valid_config) == 0);
    ck_assert(parse_args(1, args) == 0);
    ck_assert(opts.port != NULL);
    ck_assert(strcmp(opts.port, "dummy") == 0);
    ck_assert(opts.baud == 57600);
    ck_assert(opts.lecho);
    ck_assert(opts.omap == (M_CRCRLF | M_DELBS));
    ck_assert(strcmp(opts.initstring, "ati4\n") == 0);
}

START_TEST(test_read_port_from_config) {
    char *args[] = {"picocom"};

    ck_assert(parse_args(1, args) != 0);
    ck_assert(parse_config_buf(valid_config) == 0);
    ck_assert(parse_args(1, args) == 0);
}

START_TEST(test_default_configfile_respects_xdg_home) {
    setenv("XDG_CONFIG_HOME", "/test", 1);
    ck_assert(strcmp(default_config_file(), "/test/picocom/picocom.conf") == 0);
}


START_TEST(test_default_configfile_respects_home) {
    setenv("HOME", "/test", 1);
    unsetenv("XDG_CONFIG_HOME");
    ck_assert(strcmp(default_config_file(), "/test/.config/picocom/picocom.conf") == 0);
}

START_TEST(test_default_configfile_returns_null_if_no_home) {
    unsetenv("HOME");
    unsetenv("XDG_CONFIG_HOME");
    ck_assert(default_config_file() == NULL);
}
Suite * make_configfile_suite(void)
{
    Suite *s;
    TCase *tc_core;

    s = suite_create("ConfigFile");

    /* Core test case */
    tc_core = tcase_create("Core");

    tcase_add_checked_fixture(tc_core, setup, teardown);

    tcase_add_test(tc_core, test_no_error_on_missing_config);
    tcase_add_test(tc_core, test_error_on_invalid_option);
    tcase_add_test(tc_core, test_error_on_invalid_value);
    tcase_add_test(tc_core, test_no_error_on_valid_config);
    tcase_add_test(tc_core, test_setting_config_sets_option);
    tcase_add_test(tc_core, test_read_port_from_config);
    tcase_add_test(tc_core, test_default_configfile_respects_xdg_home);
    tcase_add_test(tc_core, test_default_configfile_respects_home);
    tcase_add_test(tc_core, test_default_configfile_returns_null_if_no_home);

    suite_add_tcase(s, tc_core);

    return s;
}
