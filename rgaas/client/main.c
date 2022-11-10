#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>

#include "../../modules/argparser/argparser.h"
#include "../../modules/logger/logger.h"
#include "../../modules/signal_handler/signal_handler.h"
#include "../../modules/tcp_handler/tcp_handler.h"

// NOLINTNEXTLINE(readability-function-cognitive-complexity)
int main(int argc, char **argv)
{
    argparser_client_t *argparser = argparser_client_new();
    logger_t *logger = logger_new();

    argparser->generic.parse(argparser, argc, argv);
    logger->enable_verbose_output(logger, argparser->args.generic.verbose_output);

    if (logger->open(logger, NULL, "a+") == EXIT_SUCCESS)
    {
        logger->write(logger, LOG_USER, LOG_DEBUG, "program started\n");
    }
    else
    {
        logger->write(logger, LOG_USER, LOG_ERR, "program started (logger initialization failed, logging to stdout)\n");
    }

    signal_handler_t *signal_handler = signal_handler_new();

    signal_handler->set(signal_handler);

    if (argparser->generic.status == EXIT_SUCCESS)
    {
        if (argparser->generic.non_opt_arg_found == true)
        {
            for (int i = argparser->args.generic.optind; i < argc; i++)
            {
                logger->write(logger, LOG_USER, LOG_NOTICE, "non-option argument found: %s\n", argparser->args.generic.argv[i]);
            }
        }
    }
    else
    {
        logger->write(logger, LOG_USER, LOG_NOTICE, argparser->generic.usage_info);
        logger->write(logger, LOG_USER, LOG_NOTICE, argparser->generic.usage_info_generic);
        logger->close(logger);
        logger->free(logger);
        argparser->generic.free(argparser);
        signal_handler->free(signal_handler);
        return EXIT_FAILURE;
    }

    tcp_handler_client_t * tcp_handler = tcp_handler_client_new();

    tcp_handler->generic.socket(tcp_handler);

    if (tcp_handler->generic.socket_fd == -1)
    {
        logger->write(logger, LOG_USER, LOG_ERR, "%s %s\n", "opening socket failed:", strerror(errno));
        argparser->generic.free(argparser);
        logger->close(logger);
        logger->free(logger);
        signal_handler->free(signal_handler);
        tcp_handler->generic.free(tcp_handler);
        return EXIT_FAILURE;
    }

    tcp_handler->client.gethostbyname(tcp_handler, argparser->args.client.hostname);
    if (tcp_handler->client.server == NULL)
    {
        logger->write(logger, LOG_USER, LOG_ERR, "no such host\n");
        argparser->generic.free(argparser);
        logger->close(logger);
        logger->free(logger);
        signal_handler->free(signal_handler);
        return EXIT_FAILURE;
    }

    tcp_handler->client.update_server(tcp_handler, argparser->args.generic.port_number);

    if (tcp_handler->client.connect(tcp_handler) == -1)
    {
        logger->write(logger, LOG_USER, LOG_ERR, "%s %s\n", "connect failed:", strerror(errno));
        return EXIT_FAILURE;
    }

    while (signal_flag == 0)
    {
        logger->write(logger, LOG_USER, LOG_NOTICE, "please enter the number of random bytes to be requested: \n");
        memset(tcp_handler->generic.message_buffer, 0, MESSAGE_BUFFER_SIZE);
        if (argparser->args.client.test_mode == true)
        {
            memcpy(tcp_handler->generic.message_buffer, "5", sizeof("5"));
        }
        else
        {
            (void) fgets(tcp_handler->generic.message_buffer, MESSAGE_BUFFER_SIZE, stdin);
        }

        tcp_handler->generic.write(tcp_handler);
        if (tcp_handler->generic.number_of_bytes == -1)
        {
            logger->write(logger, LOG_USER, LOG_ERR, "%s %s\n", "writing to socket failed:", strerror(errno));
            return EXIT_FAILURE;
        }

        tcp_handler->generic.read(tcp_handler);
        if (tcp_handler->generic.number_of_bytes == -1)
        {
            logger->write(logger, LOG_USER, LOG_ERR, "%s %s\n", "reading from socket failed:", strerror(errno));
            return EXIT_FAILURE;
        }

        if (tcp_handler->generic.number_of_bytes == 0)
        {
            logger->write(logger, LOG_USER, LOG_ERR, "no answer received from server, connection lost\n");
            break;
        }

        if (tcp_handler->generic.number_of_bytes > 0)
        {
            logger->write(logger, LOG_USER, LOG_NOTICE, "received from server:\n%s\n", tcp_handler->generic.message_buffer);
        }

        if (argparser->args.client.test_mode == true)
        {
            break;
        }
    }

    tcp_handler->generic.close(tcp_handler, tcp_handler->generic.socket_fd);

    logger->close(logger);
    logger->free(logger);
    argparser->generic.free(argparser);
    signal_handler->free(signal_handler);
    return EXIT_SUCCESS;
}
