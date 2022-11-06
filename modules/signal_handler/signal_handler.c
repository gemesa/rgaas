//
// Created by gemesa on 11/6/22.
//

#include <signal.h>
#include <stdlib.h>

#include "signal_handler.h"

volatile sig_atomic_t signal_flag = 0;

static void signal_handler(int signum)
{
    (void) signum;
    signal_flag = 1;
}

int register_signal_handler(void)
{
    if (signal(SIGINT, signal_handler) == SIG_ERR)
    {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}