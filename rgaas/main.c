//
// Created by gemesa on 11/2/22.
//

#include <stdlib.h>
#include <syslog.h>
#include <unistd.h>

#include "../modules/argparser/argparser.h"
#include "../modules/logger/logger.h"
#include "../modules/signal_handler/signal_handler.h"

int main(int argc, char **argv)
{
    if (register_signal_handler() == EXIT_FAILURE)
    {
        exit(EXIT_FAILURE);
    }

    argparser_t *argparser = argparser_new();

    argparser->parse(argparser, argc, argv);

    logger_t *logger = logger_new();

    logger->enable_verbose_output(logger, argparser->args.verbose_output);
    logger->enable_syslog(logger, argparser->args.syslog_enabled);

    if (logger->open(logger, argparser->args.log_file, "a+") == EXIT_SUCCESS)
    {
        logger->write(logger, "program started", LOG_USER, LOG_DEBUG);
    }
    else
    {
        logger->write(logger, "program started (logger initialization failed, logging to stdout)", LOG_USER, LOG_ERR);
    }

    if (argparser->status == EXIT_SUCCESS)
    {
        if (argparser->non_opt_arg_found == true)
        {
            logger->write(logger, "non-option argument found, please check usage info", LOG_USER, LOG_NOTICE);
        }
    }
    else
    {
        logger->write(logger, argparser->usage_info, LOG_USER, LOG_NOTICE);
        logger->close(logger);
        logger->free(logger);
        argparser->free(argparser);
        exit(EXIT_FAILURE);
    }

    if (argparser->args.process_mode == DAEMON_PROCESS)
    {
        if (daemon(0, 0) == EXIT_SUCCESS)
        {
            logger->write(logger, "switching to daemon mode successful", LOG_USER, LOG_DEBUG);
        }
        else
        {
            logger->write(logger, "switching to daemon mode failed", LOG_USER, LOG_ERR);
        }
    }

    argparser->free(argparser);

    while (!signal_flag)
    {
    }

    logger->write(logger, "program terminating...", LOG_USER, LOG_DEBUG);
    logger->close(logger);
    logger->free(logger);

    return 0;
}
