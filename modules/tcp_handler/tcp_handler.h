//
// Created by gemesa on 11/8/22.
//

#ifndef RGAAS_TCP_HANDLER_H
#define RGAAS_TCP_HANDLER_H

#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

enum
{
    EXIT_SOCKET_ERROR = 2,
    EXIT_BIND_ERROR,
    EXIT_LISTEN_ERROR,
    EXIT_CLOSE_FD_ERROR,
    EXIT_CLOSE_FD_NEW_ERROR,
    EXIT_ACCEPT_ERROR,
    EXIT_FORK_ERROR,
    EXIT_READ_ERROR,
    EXIT_WRITE_ERROR,
};

enum
{
    MESSAGE_BUFFER_SIZE = 255
};

typedef struct
{
    void (*socket)(void *self);
    int (*bind)(void *self, int port_number);
    int (*close)(void *self, int fd);
    void (*read)(void *self);
    void (*write)(void *self);
    void (*free)(void *self);
    int status;
    int socket_fd;
    ssize_t number_of_bytes;
    struct sockaddr_in server_address;
    char message_buffer[MESSAGE_BUFFER_SIZE];
} tcp_handler_generic_t;

typedef struct
{
    tcp_handler_generic_t generic;
    struct
    {
        int (*connect)(void * self);
        void (*gethostbyname)(void *self, const char *name);
        struct hostent *server;
        void (*update_server)(void*self, uint16_t hostshort);
    } client;
} tcp_handler_client_t;

typedef struct
{
    tcp_handler_generic_t generic;
    struct
    {
        int (*listen)(void *self);
        void (*accept)(void *self);
        void (*fork)(void *self);
        void (*setup)(void *self, unsigned int port_number);
        void (*loop)(void *self);
        int socket_fd_new;
        socklen_t client_length;
        int queue_length;
        int pid;
        struct sockaddr_in client_address;
    } server;
} tcp_handler_server_t;

extern tcp_handler_server_t *tcp_handler_server_new(void);
extern tcp_handler_client_t *tcp_handler_client_new(void);

#endif//RGAAS_TCP_HANDLER_H
