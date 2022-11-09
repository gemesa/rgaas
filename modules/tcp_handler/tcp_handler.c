//
// Created by gemesa on 11/8/22.
//

#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "../../modules/signal_handler/signal_handler.h"
#include "tcp_handler.h"

enum
{
    DEFAULT_PORT_NUMBER = 8000,
    DEFAULT_QUEUE_LENGTH = 5,
    CONVERSION_BASE = 10
};

static unsigned char *get_rand_bytestream(size_t num_bytes)
{
    unsigned char *stream = malloc(num_bytes);
    size_t i;

    for (i = 0; i < num_bytes; i++)
    {
        /* we ignore this warning because we do not care about the quality of the randomness */
        /* NOLINTNEXTLINE */
        stream[i] = rand();
    }

    return stream;
};

static void tcp_handler_socket(void *s)
{
    tcp_handler_t *self = s;
    self->socket_fd = socket(AF_INET, SOCK_STREAM, 0);
}

static int tcp_handler_bind(void *s, int port_number)
{
    tcp_handler_t *self = s;
    self->server_address.sin_port = htons(port_number);
    int status = bind(self->socket_fd, (struct sockaddr *) &self->server_address, sizeof(self->server_address));
    return status;
}

static int tcp_handler_listen(void *s)
{
    tcp_handler_t *self = s;
    int status = listen(self->socket_fd, self->queue_length);
    return status;
}

static int tcp_handler_close(void *s, int fd)
{
    tcp_handler_t *self = s;
    (void) self;
    int status = close(fd);
    return status;
}

static void tcp_handler_free(void *s)
{
    tcp_handler_t *self = s;
    free(self);
}

static void tcp_handler_accept(void *s)
{
    tcp_handler_t *self = s;
    self->socket_fd_new = accept(self->socket_fd, (struct sockaddr *) &self->client_address, &self->client_length);
}

static void tcp_handler_fork(void *s)
{
    tcp_handler_t *self = s;
    self->pid = fork();
}

static void tcp_handler_read(void *s)
{
    tcp_handler_t *self = s;
    memset(self->message_buffer, 0, MESSAGE_BUFFER_SIZE);
    self->number_of_bytes = read(self->socket_fd_new, self->message_buffer, MESSAGE_BUFFER_SIZE);
}

static void tcp_handler_write(void *s)
{
    tcp_handler_t *self = s;
    char *ptr;
    unsigned int number = strtoul(self->message_buffer, &ptr, CONVERSION_BASE);
    if ((number > 0) && (number <= MESSAGE_BUFFER_SIZE))
    {
        unsigned char *bytes = get_rand_bytestream(number);
        // NOLINTNEXTLINE
        self->number_of_bytes = write(self->socket_fd_new, bytes, number);
    }
    else
    {
        self->number_of_bytes = write(self->socket_fd_new, "invalid value", sizeof("invalid value"));
    }
}

static void tcp_handler_setup(void *s, unsigned int port_number)
{
    tcp_handler_t *self = s;

    self->socket(self);
    if (self->socket_fd == -1)
    {
        self->status = EXIT_SOCKET_ERROR;
        return;
    }

    if (self->bind(self, port_number) == -1)
    {
        self->status = EXIT_BIND_ERROR;
        return;
    }

    if (self->listen(self) == -1)
    {
        self->status = EXIT_LISTEN_ERROR;
        return;
    }

    self->status = EXIT_SUCCESS;
}

// NOLINTNEXTLINE(readability-function-cognitive-complexity)
void tcp_handler_loop(void *s)
{
    tcp_handler_t *self = s;

    /* while to support multiple connections */
    while (signal_flag == 0)
    {
        self->accept(self);

        /* accept is blocking and is not restarted (refer to signal_handler), break immediately if SIGINT received */
        if (signal_flag == 1)
        {
            if (self->close(self, self->socket_fd == -1))
            {
                self->status = EXIT_CLOSE_FD_ERROR;
                return;
            }

            break;
        }

        if (self->socket_fd_new == -1)
        {
            self->status = EXIT_ACCEPT_ERROR;
            return;
        }

        self->fork(self);

        if (self->pid == -1)
        {
            self->status = EXIT_FORK_ERROR;
            return;
        }

        if (self->pid == 0)
        {
            /* child process enters here */
            if (self->close(self, self->socket_fd == -1))
            {
                self->status = EXIT_CLOSE_FD_ERROR;
                return;
            }

            /* while to support multiple request per connection */
            while (signal_flag == 0)
            {
                self->read(self);

                /* read is blocking and is not restarted (refer to signal_handler), break immediately if SIGINT received */
                if (signal_flag == 1)
                {
                    break;
                }

                if (self->number_of_bytes == -1)
                {
                    self->status = EXIT_READ_ERROR;
                }

                self->write(self);

                if (self->number_of_bytes == -1)
                {
                    self->status = EXIT_WRITE_ERROR;
                }
            }

            if (self->close(self, self->socket_fd_new) == -1)
            {
                self->status = EXIT_CLOSE_FD_NEW_ERROR;
                return;
            }
        }
        else
        {
            /* parent process enters here */
            if (self->close(self, self->socket_fd_new) == -1)
            {
                self->status = EXIT_CLOSE_FD_NEW_ERROR;
                return;
            }
        }
    }

    self->status = EXIT_SUCCESS;
}

static void tcp_handler_initialize(void *s)
{
    tcp_handler_t *self = s;
    self->socket = &tcp_handler_socket;
    self->bind = &tcp_handler_bind;
    self->connect = &connect;
    self->accept = &tcp_handler_accept;
    self->listen = &tcp_handler_listen;
    self->close = &tcp_handler_close;
    self->fork = &tcp_handler_fork;
    self->read = &tcp_handler_read;
    self->write = &tcp_handler_write;
    self->free = &tcp_handler_free;
    self->setup = &tcp_handler_setup;
    self->loop = &tcp_handler_loop;
    self->status = EXIT_SUCCESS;
    self->socket_fd = -1;
    self->socket_fd_new = -1;
    self->client_length = sizeof(self->client_address);
    self->queue_length = DEFAULT_QUEUE_LENGTH;
    self->number_of_bytes = 0;
    memset(&self->server_address, 0, sizeof(self->server_address));
    self->server_address.sin_family = AF_INET;
    self->server_address.sin_addr.s_addr = INADDR_ANY;
    self->server_address.sin_port = htons(DEFAULT_PORT_NUMBER);
    memset(&self->client_address, 0, sizeof(self->client_address));
    memset(self->message_buffer, 0, MESSAGE_BUFFER_SIZE);
}

tcp_handler_t *tcp_handler_new(void)
{
    tcp_handler_t *self = malloc(sizeof(tcp_handler_t));
    tcp_handler_initialize(self);
    return self;
}