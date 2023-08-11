#include <stdio.h>
#include <check.h>

#include "picocom.h"
#include "configfile.h"

extern int parse_args(int argc, char *argv[]);
extern void init_defaults(void);

static void setup(void) {
    init_config();
    init_defaults();
}

static void teardown(void) {}

struct int_opt_int_val_paramspec {
    char *option;
    int argument;
    int *value;
};

struct str_opt_int_val_paramspec {
    char *option;
    char *argument;
    int want;
    int *value;
};

struct str_opt_str_val_paramspec {
    char *option;
    char *argument;
    char **value;
};

static struct int_opt_int_val_paramspec int_opt_int_val_params[] = {
    {"-b", 57600, &opts.baud},
    {"--baud", 57600, &opts.baud},
    {"-d", 5, &opts.databits},
    {"--databits", 5, &opts.databits},
    {"-p", 2, &opts.stopbits},
    {"--stopbits", 2, &opts.stopbits},
};

static struct int_opt_int_val_paramspec flag_opt_int_val_params[] = {
    {"-n", 0, (int *)&opts.escape},
    {"--no-escape", 0, (int *)&opts.escape},
    {"-c", 1, &opts.lecho},
    {"--echo", 1, &opts.lecho},
    {"-i", 1, &opts.noinit},
    {"--noinit", 1, &opts.noinit},
    {"-r", 1, &opts.noreset},
    {"--noreset", 1, &opts.noreset},
    {"-u", 1, &opts.hangup},
    {"--hangup", 1, &opts.hangup},
    {"-l", 1, &opts.nolock},
    {"--nolock", 1, &opts.nolock},
    {"-X", 1, &opts.exit},
    {"--exit", 1, &opts.exit},
    {"-q", 1, &opts.quiet},
    {"--quiet", 1, &opts.quiet},
    {"--excl", 1, &opts.excl},
    {"--lower-rts", RTS_DTR_LOWER, &opts.raise_lower_rts},
    {"--raise-rts", RTS_DTR_RAISE, &opts.raise_lower_rts},
    {"--lower-dtr", RTS_DTR_LOWER, &opts.raise_lower_dtr},
    {"--raise-dtr", RTS_DTR_RAISE, &opts.raise_lower_dtr},
};

static struct str_opt_int_val_paramspec str_opt_int_val_params[] = {
    {"-f", "x", FC_XONXOFF, (int *)&opts.flow},
    {"-f", "h", FC_RTSCTS, (int *)&opts.flow},
    {"-f", "n", FC_NONE, (int *)&opts.flow},
    {"-f", "soft", FC_XONXOFF, (int *)&opts.flow},
    {"-f", "hard", FC_RTSCTS, (int *)&opts.flow},
    {"-f", "none", FC_NONE, (int *)&opts.flow},
    {"-y", "o", P_ODD, (int *)&opts.parity},
    {"-y", "e", P_EVEN, (int *)&opts.parity},
    {"-y", "n", P_NONE, (int *)&opts.parity},
    {"-y", "odd", P_ODD, (int *)&opts.parity},
    {"-y", "even", P_EVEN, (int *)&opts.parity},
    {"-y", "none", P_NONE, (int *)&opts.parity},
    {"--imap", "crlf,delbs", M_CRLF|M_DELBS, &opts.imap},
    {"--omap", "crlf,delbs", M_CRLF|M_DELBS, &opts.omap},
    {"--emap", "crlf,delbs", M_CRLF|M_DELBS, &opts.emap},
    {"--rts", "none", RTS_DTR_NONE, &opts.raise_lower_rts},
    {"--rts", "raise", RTS_DTR_RAISE, &opts.raise_lower_rts},
    {"--rts", "lower", RTS_DTR_LOWER, &opts.raise_lower_rts},
    {"--dtr", "none", RTS_DTR_NONE, &opts.raise_lower_dtr},
    {"--dtr", "raise", RTS_DTR_RAISE, &opts.raise_lower_dtr},
    {"--dtr", "lower", RTS_DTR_LOWER, &opts.raise_lower_dtr},
};

static struct str_opt_str_val_paramspec str_opt_str_val_params[] = {
    {"--send-cmd", "test", &opts.send_cmd},
    {"--receive-cmd", "test", &opts.receive_cmd},
    {"--initstring", "test", &opts.initstring},
    {"--logfile", "test", &opts.log_filename},
};

START_TEST(test_int_opt_int_val) {
    struct int_opt_int_val_paramspec param = int_opt_int_val_params[_i];
    char value[20];
    snprintf(value, 20, "%d", param.argument);
    char *args[] = {"picocom", param.option, value, "dummy"};
    ck_assert(parse_args(4, args) == 0);
    ck_assert(*(param.value) == param.argument);
}

START_TEST(test_flag_opt_int_val) {
    struct int_opt_int_val_paramspec param = flag_opt_int_val_params[_i];
    char *args[] = {"picocom", param.option, "dummy"};
    ck_assert(parse_args(3, args) == 0);
    ck_assert(*(param.value) == param.argument);
}

START_TEST(test_str_opt_int_val) {
    struct str_opt_int_val_paramspec param = str_opt_int_val_params[_i];
    char *args[] = {"picocom", param.option, NULL, "dummy"};
    char *tmp;

    // This dance is necessary because options processing assumes that
    // optarg is writable.
    tmp = malloc(strlen(param.argument));
    strcpy(tmp, param.argument);
    args[2] = tmp;
    ck_assert(parse_args(4, args) == 0);
    free(tmp);
    ck_assert(*(param.value) == param.want);
}

START_TEST(test_str_opt_str_val) {
    struct str_opt_str_val_paramspec param = str_opt_str_val_params[_i];

    char *args[] = {"picocom", param.option, param.argument, "dummy"};
    ck_assert(parse_args(4, args) == 0);
    ck_assert(strcmp(*(param.value), param.argument) == 0);
}

Suite * make_options_suite(void)
{
    Suite *s;
    TCase *tc_core;
    int nr_tests;

    s = suite_create("Options");

    /* Core test case */
    tc_core = tcase_create("Core");

    tcase_add_checked_fixture(tc_core, setup, teardown);

    // count number of test cases
    nr_tests = sizeof(int_opt_int_val_params)/sizeof(struct int_opt_int_val_paramspec);
    tcase_add_loop_test(tc_core, test_int_opt_int_val, 0, nr_tests);

    nr_tests = sizeof(flag_opt_int_val_params)/sizeof(struct int_opt_int_val_paramspec);
    tcase_add_loop_test(tc_core, test_flag_opt_int_val, 0, nr_tests);

    nr_tests = sizeof(str_opt_int_val_params)/sizeof(struct str_opt_int_val_paramspec);
    tcase_add_loop_test(tc_core, test_str_opt_int_val, 0, nr_tests);

    nr_tests = sizeof(str_opt_str_val_params)/sizeof(struct str_opt_str_val_paramspec);
    tcase_add_loop_test(tc_core, test_str_opt_str_val, 0, nr_tests);

    suite_add_tcase(s, tc_core);

    return s;
}
