#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <syslog.h>
#include <unistd.h>

#include "../../modules/argparser/argparser.h"
#include "../../modules/logger/logger.h"
#include "../../modules/signal_handler/signal_handler.h"
#include "../../modules/tcp_handler/tcp_handler.h"

// NOLINTNEXTLINE(readability-function-cognitive-complexity)
int main(int argc, char **argv)
{
    argparser_t *argparser = argparser_new();
    logger_t *logger = logger_new();

    argparser->parse(argparser, argc, argv);
    logger->enable_verbose_output(logger, argparser->args.verbose_output);

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

    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd == -1)
    {
        logger->write(logger, LOG_USER, LOG_ERR, "%s %s\n", "opening socket failed:", strerror(errno));
        argparser->free(argparser);
        logger->close(logger);
        logger->free(logger);
        signal_handler->free(signal_handler);
        return EXIT_FAILURE;
    }

    struct hostent *server = gethostbyname(argparser->args.hostname);
    if (server == NULL)
    {
        logger->write(logger, LOG_USER, LOG_ERR, "no such host\n");
        argparser->free(argparser);
        logger->close(logger);
        logger->free(logger);
        signal_handler->free(signal_handler);
        return EXIT_FAILURE;
    }

    struct sockaddr_in server_address;
    memset((char *) &server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    memcpy((char *) &server_address.sin_addr.s_addr,
           (char *) server->h_addr,
           server->h_length);
    server_address.sin_port = htons(argparser->args.port_number);

    if (connect(socket_fd, (struct sockaddr *) &server_address, sizeof(server_address)) == -1)
    {
        logger->write(logger, LOG_USER, LOG_ERR, "%s %s\n", "connect failed:", strerror(errno));
        return EXIT_FAILURE;
    }

    char message_buffer[MESSAGE_BUFFER_SIZE];
    int num_of_bytes;
    while (signal_flag == 0)
    {
        logger->write(logger, LOG_USER, LOG_NOTICE, "please enter the number of random bytes to be requested: \n");
        memset(message_buffer, 0, MESSAGE_BUFFER_SIZE);
        if (argparser->args.test_mode == true)
        {
            memcpy(message_buffer, "5", sizeof("5"));
        }
        else
        {
            (void) fgets(message_buffer, MESSAGE_BUFFER_SIZE, stdin);
        }

        num_of_bytes = write(socket_fd, message_buffer, sizeof(message_buffer));
        if (num_of_bytes == -1)
        {
            logger->write(logger, LOG_USER, LOG_ERR, "%s %s\n", "writing to socket failed:", strerror(errno));
            return EXIT_FAILURE;
        }

        memset(message_buffer, 0, MESSAGE_BUFFER_SIZE);
        num_of_bytes = read(socket_fd, message_buffer, MESSAGE_BUFFER_SIZE);
        if (num_of_bytes == -1)
        {
            logger->write(logger, LOG_USER, LOG_ERR, "%s %s\n", "reading from socket failed:", strerror(errno));
            return EXIT_FAILURE;
        }

        if (num_of_bytes == 0)
        {
            logger->write(logger, LOG_USER, LOG_ERR, "no answer received from server, connection lost\n");
            break;
        }

        if (num_of_bytes > 0)
        {
            logger->write(logger, LOG_USER, LOG_NOTICE, "received from server:\n%s\n", message_buffer);
        }

        if (argparser->args.test_mode == true)
        {
            break;
        }
    }

    close(socket_fd);

    logger->close(logger);
    logger->free(logger);
    argparser->free(argparser);
    signal_handler->free(signal_handler);
    return EXIT_SUCCESS;
}
