//
// Created by gemesa on 11/2/22.
//

#include <stdlib.h>
#include <syslog.h>

#include "../modules/argparser/argparser.h"
#include "../modules/logger/logger.h"
#include "../modules/signal_handler/signal_handler.h"

int main(int argc, char **argv)
{
    if (register_signal_handler() == EXIT_FAILURE)
    {
        exit(EXIT_FAILURE);
    }

    args_t args = argparse(argc, argv);

    logger_t *logger = logger_new();

    logger->enable_syslog(logger, args.syslog_enabled);

    if (logger->open(logger, args.log_file, "a+") == EXIT_SUCCESS)
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
