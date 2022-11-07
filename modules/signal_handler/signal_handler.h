//
// Created by gemesa on 11/6/22.
//

#ifndef RGAAS_SIGNAL_HANDLER_H
#define RGAAS_SIGNAL_HANDLER_H

#include <signal.h>

typedef struct
{
    int (*set)(void *self);
    void (*handle)(int signal);
    void (*free)(void *self);
} signal_handler_t;

extern signal_handler_t *signal_handler_new(void);
extern volatile sig_atomic_t signal_flag;

#endif//RGAAS_SIGNAL_HANDLER_H
