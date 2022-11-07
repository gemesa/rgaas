//
// Created by gemesa on 11/2/22.
//

#include <stdlib.h>
#include <syslog.h>

#include "../modules/logger/logger.h"
#include "../modules/signal_handler/signal_handler.h"

int main(void)
{
    if (register_signal_handler() == EXIT_FAILURE)
    {
        exit(EXIT_FAILURE);
    }

    logger_t *logger = logger_new();

    if (logger->open(logger, NULL, "a+") == EXIT_SUCCESS)
    {
        logger->write(logger, "program started", LOG_USER, LOG_NOTICE);
    }
    else
    {
        logger->write(logger, "program started (logger initialization failed, logging to stdout)", LOG_USER, LOG_ERR);
    }

    while (!signal_flag)
    {
    }

    logger->write(logger, "program terminating...", LOG_USER, LOG_NOTICE);
    logger->close(logger);
    logger->free(logger);

    return 0;
}
