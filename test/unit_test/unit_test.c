//
// Created by gemesa on 11/10/22.
//

#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "cmocka/include/cmocka.h"

#include "argparser_unit_test.h"
#include "logger_unit_test.h"
#include "tcp_handler_unit_test.h"

int main()
{
    const struct CMUnitTest argparser[] = {
            cmocka_unit_test(verify_that_client_argc_argv_parsed_properly),
            cmocka_unit_test(verify_that_client_argparser_initialized_properly),
            cmocka_unit_test(verify_that_server_argc_argv_parsed_properly),
            cmocka_unit_test(verify_that_server_argparser_initialized_properly),
    };

    const struct CMUnitTest logger[] = {
            cmocka_unit_test(verify_that_logger_writes_verbose_properly),
            cmocka_unit_test(verify_that_logger_writes_nonverbose_properly),
    };

    const struct CMUnitTest tcp_handler[] = {
            cmocka_unit_test(verify_that_client_tcp_handler_initialized_properly),
            cmocka_unit_test(verify_that_server_tcp_handler_initialized_properly),
    };

    int status = 0;
    status += cmocka_run_group_tests(argparser, NULL, NULL);
    status += cmocka_run_group_tests(logger, NULL, NULL);
    status += cmocka_run_group_tests(tcp_handler, NULL, NULL);
    return status;
}