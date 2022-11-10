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
        /* NOLINTNEXTLINE(cert-msc30-c,cert-msc50-cpp) */
        stream[i] = rand();
    }

    return stream;
};

static void tcp_handler_socket(void *s)
{
    tcp_handler_generic_t *self = s;
    self->socket_fd = socket(AF_INET, SOCK_STREAM, 0);
}

static int tcp_handler_bind(void *s, int port_number)
{
    tcp_handler_generic_t *self = s;
    self->server_address.sin_port = htons(port_number);
    int status = bind(self->socket_fd, (struct sockaddr *) &self->server_address, sizeof(self->server_address));
    return status;
}

static int tcp_handler_listen(void *s)
{
    tcp_handler_server_t *self = s;
    int status = listen(self->generic.socket_fd, self->server.queue_length);
    return status;
}

static int tcp_handler_close(void *s, int fd)
{
    tcp_handler_generic_t *self = s;
    (void) self;
    int status = close(fd);
    return status;
}

static void tcp_handler_free(void *s)
{
    tcp_handler_generic_t *self = s;
    free(self);
}

static void tcp_handler_accept(void *s)
{
    tcp_handler_server_t *self = s;
    self->server.socket_fd_new = accept(self->generic.socket_fd, (struct sockaddr *) &self->server.client_address, &self->server.client_length);
}

static void tcp_handler_fork(void *s)
{
    tcp_handler_server_t *self = s;
    self->server.pid = fork();
}

static void tcp_handler_client_read(void *s)
{
    tcp_handler_client_t *self = s;
    memset(self->generic.message_buffer, 0, MESSAGE_BUFFER_SIZE);
    self->generic.number_of_bytes = read(self->generic.socket_fd, self->generic.message_buffer, MESSAGE_BUFFER_SIZE);
}

static void tcp_handler_server_read(void *s)
{
    tcp_handler_server_t *self = s;
    memset(self->generic.message_buffer, 0, MESSAGE_BUFFER_SIZE);
    self->generic.number_of_bytes = read(self->server.socket_fd_new, self->generic.message_buffer, MESSAGE_BUFFER_SIZE);
}

static void tcp_handler_write(void *s)
{
    tcp_handler_generic_t *self = s;
    self->number_of_bytes = write(self->socket_fd, self->message_buffer, sizeof(self->message_buffer));
}

static void tcp_handler_server_write(void *s)
{
    tcp_handler_server_t *self = s;
    char *ptr;
    unsigned int number = strtoul(self->generic.message_buffer, &ptr, CONVERSION_BASE);
    if ((number > 0) && (number <= MESSAGE_BUFFER_SIZE))
    {
        unsigned char *bytes = get_rand_bytestream(number);

        /* NOLINTNEXTLINE(clang-analyzer-unix.Malloc) */
        self->generic.number_of_bytes = write(self->server.socket_fd_new, bytes, number);
    }
    else
    {
        self->generic.number_of_bytes = write(self->server.socket_fd_new, "invalid value", sizeof("invalid value"));
    }
}

static void tcp_handler_setup(void *s, unsigned int port_number)
{
    tcp_handler_server_t *self = s;

    self->generic.socket(self);
    if (self->generic.socket_fd == -1)
    {
        self->generic.status = EXIT_SOCKET_ERROR;
        return;
    }

    if (self->generic.bind(self, port_number) == -1)
    {
        self->generic.status = EXIT_BIND_ERROR;
        return;
    }

    if (self->server.listen(self) == -1)
    {
        self->generic.status = EXIT_LISTEN_ERROR;
        return;
    }

    self->generic.status = EXIT_SUCCESS;
}

static void tcp_handler_gethostbyname(void *s, const char* name)
{
    tcp_handler_client_t * self = s;
    self->client.server = gethostbyname(name);
}

static void tcp_handler_update_server_address(void*s, uint16_t hostshort)
{
    tcp_handler_client_t * self = s;
    memcpy((char *) &self->generic.server_address.sin_addr.s_addr,
           (char *) self->client.server->h_addr,
           self->client.server->h_length);
    self->generic.server_address.sin_port = htons(hostshort);
}

static int tcp_handler_connect(void * s)
{
    tcp_handler_client_t * self = s;
    int status =  connect(self->generic.socket_fd, (struct sockaddr *) &self->generic.server_address, sizeof(self->generic.server_address));
    return status;
}

/* NOLINTNEXTLINE(readability-function-cognitive-complexity) */
void tcp_handler_loop(void *s)
{
    tcp_handler_server_t *self = s;

    /* while to support multiple connections */
    while (signal_flag == 0)
    {
        self->server.accept(self);

        /* accept is blocking and is not restarted (refer to signal_handler), break immediately if SIGINT received */
        if (signal_flag == 1)
        {
            if (self->generic.close(self, self->generic.socket_fd == -1))
            {
                self->generic.status = EXIT_CLOSE_FD_ERROR;
                return;
            }

            break;
        }

        if (self->server.socket_fd_new == -1)
        {
            self->generic.status = EXIT_ACCEPT_ERROR;
            return;
        }

        self->server.fork(self);

        if (self->server.pid == -1)
        {
            self->generic.status = EXIT_FORK_ERROR;
            return;
        }

        if (self->server.pid == 0)
        {
            /* child process enters here */
            if (self->generic.close(self, self->generic.socket_fd == -1))
            {
                self->generic.status = EXIT_CLOSE_FD_ERROR;
                return;
            }

            /* while to support multiple request per connection */
            while (signal_flag == 0)
            {
                self->generic.read(self);

                /* read is blocking and is not restarted (refer to signal_handler), break immediately if SIGINT received */
                if (signal_flag == 1)
                {
                    break;
                }

                if (self->generic.number_of_bytes == -1)
                {
                    self->generic.status = EXIT_READ_ERROR;
                }

                self->generic.write(self);

                if (self->generic.number_of_bytes == -1)
                {
                    self->generic.status = EXIT_WRITE_ERROR;
                }
            }

            if (self->generic.close(self, self->server.socket_fd_new) == -1)
            {
                self->generic.status = EXIT_CLOSE_FD_NEW_ERROR;
                return;
            }
        }
        else
        {
            /* parent process enters here */
            if (self->generic.close(self, self->server.socket_fd_new) == -1)
            {
                self->generic.status = EXIT_CLOSE_FD_NEW_ERROR;
                return;
            }
        }
    }

    self->generic.status = EXIT_SUCCESS;
}

static void tcp_handler_generic_initialize(void *s)
{
    tcp_handler_generic_t *self = s;
    self->socket = &tcp_handler_socket;
    self->bind = &tcp_handler_bind;
    self->close = &tcp_handler_close;
    self->free = &tcp_handler_free;
    self->status = EXIT_SUCCESS;
    self->socket_fd = -1;
    self->number_of_bytes = 0;
    memset(self->message_buffer, 0, MESSAGE_BUFFER_SIZE);
}

static void tcp_handler_client_initialize(void *s)
{
    tcp_handler_client_t *self = s;
    self->client.gethostbyname = &tcp_handler_gethostbyname;
    self->client.update_server = &tcp_handler_update_server_address;
    self->generic.write = &tcp_handler_write;
    self->generic.read = &tcp_handler_client_read;
    self->client.connect = &tcp_handler_connect;
    self->client.server = NULL;
    memset((char *) &self->generic.server_address, 0, sizeof(self->generic.server_address));
    self->generic.server_address.sin_family = AF_INET;
}

static void tcp_handler_server_initialize(void *s)
{
    tcp_handler_server_t *self = s;
    self->generic.write = &tcp_handler_server_write;
    self->generic.read = &tcp_handler_server_read;
    self->server.listen = &tcp_handler_listen;
    self->server.accept = &tcp_handler_accept;
    self->server.fork = &tcp_handler_fork;
    self->server.setup = &tcp_handler_setup;
    self->server.loop = &tcp_handler_loop;
    self->server.socket_fd_new = -1;
    self->server.client_length = sizeof(self->server.client_address);
    self->server.queue_length = DEFAULT_QUEUE_LENGTH;
    memset(&self->generic.server_address, 0, sizeof(self->generic.server_address));
    self->generic.server_address.sin_family = AF_INET;
    self->generic.server_address.sin_addr.s_addr = INADDR_ANY;
    self->generic.server_address.sin_port = htons(DEFAULT_PORT_NUMBER);
    memset(&self->server.client_address, 0, sizeof(self->server.client_address));
}

tcp_handler_client_t *tcp_handler_client_new(void)
{
    tcp_handler_client_t *self = malloc(sizeof(tcp_handler_client_t));
    tcp_handler_generic_initialize(self);
    tcp_handler_client_initialize(self);
    return self;
}

tcp_handler_server_t *tcp_handler_server_new(void)
{
    tcp_handler_server_t *self = malloc(sizeof(tcp_handler_server_t));
    tcp_handler_generic_initialize(self);
    tcp_handler_server_initialize(self);
    return self;
}