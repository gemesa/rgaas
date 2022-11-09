//
// Created by gemesa on 11/2/22.
//

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <unistd.h>

#include "../../modules/argparser/argparser.h"
#include "../../modules/logger/logger.h"
#include "../../modules/signal_handler/signal_handler.h"
#include "../../modules/tcp/tcp_handler.h"

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
                logger->write(logger, LOG_USER, LOG_NOTICE, "non-option argument found: %s\n", argparser->args.argv[i]);
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

    tcp_handler_t *tcp_handler = tcp_handler_new();

    tcp_handler->setup(tcp_handler, argparser->args.port_number);
    switch (tcp_handler->status)
    {
        case EXIT_SOCKET_ERROR:
            logger->write(logger, LOG_USER, LOG_ERR, "%s %s\n", "opening socket failed:", strerror(errno));
            break;
        case EXIT_BIND_ERROR:
            logger->write(logger, LOG_USER, LOG_ERR, "%s %s\n", "binding failed:", strerror(errno));
            break;
        case EXIT_LISTEN_ERROR:
            logger->write(logger, LOG_USER, LOG_ERR, "%s %s\n", "listening failed:", strerror(errno));
            break;
        default:
            logger->write(logger, LOG_USER, LOG_DEBUG, "tcp setup successful\n");
            break;
    }

    tcp_handler->loop(tcp_handler);
    switch (tcp_handler->status)
    {
        case EXIT_CLOSE_FD_ERROR:
            logger->write(logger, LOG_USER, LOG_ERR, "%s %s\n", "closing socket_fd failed:", strerror(errno));
            break;
        case EXIT_CLOSE_FD_NEW_ERROR:
            logger->write(logger, LOG_USER, LOG_ERR, "%s %s\n", "closing socket_fd_new failed:", strerror(errno));
            break;
        case EXIT_ACCEPT_ERROR:
            logger->write(logger, LOG_USER, LOG_ERR, "%s %s\n", "accept failed:", strerror(errno));
            break;
        case EXIT_FORK_ERROR:
            logger->write(logger, LOG_USER, LOG_ERR, "%s %s\n", "fork failed:", strerror(errno));
            break;
        case EXIT_READ_ERROR:
            logger->write(logger, LOG_USER, LOG_ERR, "%s %s\n", "reading from socket failed:", strerror(errno));
            break;
        case EXIT_WRITE_ERROR:
            logger->write(logger, LOG_USER, LOG_ERR, "%s %s\n", "writing to socket failed:", strerror(errno));
            break;
        default:
            logger->write(logger, LOG_USER, LOG_DEBUG, "tcp loop successful\n");
            break;
    }

    logger->write(logger, LOG_USER, LOG_DEBUG, "program terminating...\n");
    logger->close(logger);
    logger->free(logger);
    argparser->free(argparser);
    signal_handler->free(signal_handler);
    tcp_handler->free(tcp_handler);

    return EXIT_SUCCESS;
}
