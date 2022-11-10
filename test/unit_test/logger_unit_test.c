//
// Created by gemesa on 11/10/22.
//


#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <stdlib.h>
#include <syslog.h>

#include "cmocka/include/cmocka.h"

#include "../../modules/logger/logger.h"

void verify_that_logger_writes_verbose_properly(void** state)
{
    (void)state;
    logger_t *logger = logger_new();
    logger->enable_verbose_output(logger, true);

    enum
    {
        SIZE_OF_BUFFER = 50
    };

    char buffer[SIZE_OF_BUFFER];
    memset(buffer, '\0', sizeof(buffer));

    if (logger->open(logger, NULL, "a+") == EXIT_SUCCESS)
    {
        (void)freopen("/dev/null", "a", stdout);
        setbuf(stdout, buffer);
        logger->write(logger, LOG_USER, LOG_DEBUG, "program started");
        (void)freopen ("/dev/tty", "a", stdout);
    }

    assert_string_equal(buffer, "15 rgaas - program started");

    logger->close(logger);
    logger->free(logger);
}

void verify_that_logger_writes_nonverbose_properly(void** state)
{
    (void)state;
    logger_t *logger = logger_new();
    logger->enable_verbose_output(logger, false);

    enum
    {
        SIZE_OF_BUFFER = 50
    };

    char buffer[SIZE_OF_BUFFER];
    memset(buffer, '\0', sizeof(buffer));

    if (logger->open(logger, NULL, "a+") == EXIT_SUCCESS)
    {
        (void)freopen("/dev/null", "a", stdout);
        setbuf(stdout, buffer);
        logger->write(logger, LOG_USER, LOG_ERR, "program terminated");
        (void)freopen ("/dev/tty", "a", stdout);
    }

    assert_string_equal(buffer, "program terminated");

    logger->close(logger);
    logger->free(logger);
}
