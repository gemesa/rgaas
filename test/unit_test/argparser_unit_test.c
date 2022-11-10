//
// Created by gemesa on 11/10/22.
//

#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <unistd.h>

#include "cmocka/include/cmocka.h"

#include "../../modules/argparser/argparser.h"

void verify_that_client_argc_argv_parsed_properly(void **state)
{
    (void) state;
    static char *argv0[] = {"unit-test", "-p", "8000", "-n", "localhost", "-v", "-t"};
    static int argc = sizeof(argv0) / sizeof(argv0[0]);
    argparser_client_t *argparser = argparser_client_new();

    optind = 1; // reset getopt()
    argparser->generic.parse(argparser, argc, argv0);

    assert_int_equal(argparser->args.generic.port_number, 8000);
    assert_string_equal(argparser->args.client.hostname, "localhost");
    assert_true(argparser->args.generic.verbose_output);
    assert_true(argparser->args.client.test_mode);

    argparser->generic.free(argparser);
}

void verify_that_client_argparser_initialized_properly(void **state)
{
    (void) state;
    char *argv[] = {"unit-test"};
    int argc = sizeof(argv) / sizeof(argv[0]);
    argparser_client_t *argparser = argparser_client_new();

    optind = 1; // reset getopt()
    argparser->generic.parse(argparser, argc, argv);

    assert_int_equal(argparser->args.generic.port_number, 8000);
    assert_string_equal(argparser->args.client.hostname, "localhost");
    assert_false(argparser->args.generic.verbose_output);
    assert_false(argparser->args.client.test_mode);

    argparser->generic.free(argparser);
}

void verify_that_server_argc_argv_parsed_properly(void **state)
{
    (void) state;
    static char *argv0[] = {"unit-test", "-p", "8000", "-l", "log.txt", "-v", "-s", "-d"};
    static int argc = sizeof(argv0) / sizeof(argv0[0]);
    argparser_server_t *argparser = argparser_server_new();

    optind = 1; // reset getopt()
    argparser->generic.parse(argparser, argc, argv0);

    assert_int_equal(argparser->args.generic.port_number, 8000);
    assert_string_equal(argparser->args.server.log_file, "log.txt");
    assert_true(argparser->args.generic.verbose_output);
    assert_true(argparser->args.server.syslog_enabled);
    assert_int_equal(argparser->args.server.process_mode, DAEMON_PROCESS);

    argparser->generic.free(argparser);
}

void verify_that_server_argparser_initialized_properly(void **state)
{
    (void) state;
    char *argv[] = {"unit-test"};
    int argc = sizeof(argv) / sizeof(argv[0]);
    argparser_server_t *argparser = argparser_server_new();

    optind = 1; // reset getopt()
    argparser->generic.parse(argparser, argc, argv);

    assert_int_equal(argparser->args.generic.port_number, 8000);
    assert_null(argparser->args.server.log_file);
    assert_false(argparser->args.generic.verbose_output);
    assert_false(argparser->args.server.syslog_enabled);
    assert_int_equal(argparser->args.server.process_mode, FOREGROUND_PROCESS);

    argparser->generic.free(argparser);
}