//
// Created by gemesa on 11/10/22.
//

#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <unistd.h>

#include "cmocka/include/cmocka.h"

#include "../../modules/tcp_handler/tcp_handler.h"

void verify_that_client_tcp_handler_initialized_properly(void **state)
{
    (void) state;
    tcp_handler_client_t *tcp_handler = tcp_handler_client_new();

    assert_null(tcp_handler->client.server);
    assert_int_equal(tcp_handler->generic.server_address.sin_family, AF_INET);

    tcp_handler->generic.free(tcp_handler);
}

void verify_that_server_tcp_handler_initialized_properly(void **state)
{
    (void) state;
    tcp_handler_server_t *tcp_handler = tcp_handler_server_new();

    enum
    {
        DEFAULT_PORT_NUMBER = 8000,
        DEFAULT_QUEUE_LENGTH = 5,
    };

    assert_int_equal(tcp_handler->server.socket_fd_new, -1);
    assert_int_equal(tcp_handler->server.client_length, sizeof(tcp_handler->server.client_address));
    assert_int_equal(tcp_handler->server.queue_length, DEFAULT_QUEUE_LENGTH);
    assert_int_equal(tcp_handler->generic.server_address.sin_family, AF_INET);
    assert_int_equal(tcp_handler->generic.server_address.sin_addr.s_addr, INADDR_ANY);
    assert_int_equal(tcp_handler->generic.server_address.sin_port, htons(DEFAULT_PORT_NUMBER));

    char empty_block[sizeof(tcp_handler->server.client_address)];
    memset(empty_block, 0, sizeof empty_block);
    assert_true(!memcmp(empty_block, &tcp_handler->server.client_address, sizeof(tcp_handler->server.client_address)));

    tcp_handler->generic.free(tcp_handler);
}