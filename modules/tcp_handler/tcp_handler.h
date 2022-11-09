//
// Created by gemesa on 11/8/22.
//

#ifndef RGAAS_TCP_HANDLER_H
#define RGAAS_TCP_HANDLER_H

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
    int (*connect)(int fd, const struct sockaddr *addr, socklen_t len);
    void (*accept)(void *self);
    int (*listen)(void *self);
    int (*close)(void *self, int fd);
    void (*fork)(void *self);
    void (*read)(void *self);
    void (*write)(void *self);
    void (*free)(void *self);
    void (*setup)(void *self, unsigned int port_number);
    void (*loop)(void *self);
    int status;
    int socket_fd;
    int socket_fd_new;
    socklen_t client_length;
    int queue_length;
    int pid;
    ssize_t number_of_bytes;
    struct sockaddr_in server_address;
    struct sockaddr_in client_address;
    char message_buffer[MESSAGE_BUFFER_SIZE];
} tcp_handler_t;

extern tcp_handler_t *tcp_handler_new(void);

#endif//RGAAS_TCP_HANDLER_H
