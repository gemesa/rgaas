//
// Created by gemesa on 11/2/22.
//

#include <stdlib.h>
#include <syslog.h>
#include <unistd.h>

#include "../../modules/argparser/argparser.h"
#include "../../modules/logger/logger.h"
#include "../../modules/signal_handler/signal_handler.h"

int main(int argc, char **argv)
{
    argparser_t *argparser = argparser_new();
    logger_t *logger = logger_new();

    argparser->parse(argparser, argc, argv);
    logger->enable_verbose_output(logger, argparser->args.verbose_output);
    logger->enable_syslog(logger, argparser->args.syslog_enabled);

    if (logger->open(logger, argparser->args.log_file, "a+") == EXIT_SUCCESS)
    {
        logger->write(logger, LOG_USER, LOG_DEBUG, "program started\n");
    }
    else
    {
        logger->write(logger, LOG_USER, LOG_ERR, "program started (logger initialization failed, logging to stdout)\n");
    }

    signal_handler_t *signal_handler = signal_handler_new();

    signal_handler->set(signal_handler);

    if (argparser->status == EXIT_SUCCESS)
    {
        if (argparser->non_opt_arg_found == true)
        {
            for (int i = argparser->args.optind; i < argc; i++)
            {
                logger->write(logger, LOG_USER, LOG_NOTICE, "non-option argument found: %s\n", argparser->args.argv[argparser->args.optind]);
            }
        }
    }
    else
    {
        logger->write(logger, LOG_USER, LOG_NOTICE, argparser->usage_info);
        logger->close(logger);
        logger->free(logger);
        argparser->free(argparser);
        signal_handler->free(signal_handler);
        return EXIT_FAILURE;
    }

    if (argparser->args.process_mode == DAEMON_PROCESS)
    {
        if (daemon(0, 0) == EXIT_SUCCESS)
        {
            logger->write(logger, LOG_USER, LOG_DEBUG, "switching to daemon mode successful\n");
        }
        else
        {
            logger->write(logger, LOG_USER, LOG_ERR, "switching to daemon mode failed\n");
        }
    }

    argparser->free(argparser);

    while (signal_flag == 0)
    {
    }

    logger->write(logger, LOG_USER, LOG_DEBUG, "program terminating...\n");
    logger->close(logger);
    logger->free(logger);
    signal_handler->free(signal_handler);

    return EXIT_SUCCESS;
}
