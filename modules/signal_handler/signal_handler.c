//
// Created by gemesa on 11/6/22.
//

#include <signal.h>
#include <stdlib.h>

#include "signal_handler.h"

volatile sig_atomic_t signal_flag = 0;

static void signal_handler_handle(int signal)
{
    (void) signal;
    signal_flag = 1;
}

static int signal_handler_set(void *s)
{
    signal_handler_t *self = s;
    if (signal(SIGINT, self->handle) == SIG_ERR)
    {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

static void signal_handler_free(void *s)
{
    signal_handler_t *self = s;
    free(self);
}

static void signal_handler_initialize(void *s)
{
    signal_handler_t *self = s;
    self->set = &signal_handler_set;
    self->handle = &signal_handler_handle;
    self->free = &signal_handler_free;
    signal_flag = 0;
}

signal_handler_t *signal_handler_new(void)
{
    signal_handler_t *self = malloc(sizeof(signal_handler_t));
    signal_handler_initialize(self);
    return self;
}