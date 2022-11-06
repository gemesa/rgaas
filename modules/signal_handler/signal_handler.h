//
// Created by gemesa on 11/6/22.
//

#ifndef RGAAS_SIGNAL_HANDLER_H
#define RGAAS_SIGNAL_HANDLER_H

#include <signal.h>

extern volatile sig_atomic_t signal_flag;
extern int register_signal_handler(void);

#endif//RGAAS_SIGNAL_HANDLER_H
