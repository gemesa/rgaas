//
// Created by gemesa on 11/3/22.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "logger.h"

const messages_t messages = {.start = "program started",
                             .start_logger_failed = "program started (logger initialization failed, logging to stdout)",
                             .terminate = "program terminating..."};

static int logger_open(void *s, char *filename, char *modes)
{
    logger_t *self = s;
    if (!filename)
    {
        self->file = stdout;
        return EXIT_SUCCESS;
    }
    self->file = fopen(filename, modes);
    if (!self->file)
    {
        self->file = stdout;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

static int logger_write(void *s, char *msg)
{
    logger_t *self = s;
    int status = fprintf(self->file, "rgaas - %s\n", msg);
    return status;
}

static int logger_close(void *s)
{
    logger_t *self = s;
    int status = fclose(self->file);
    return status;
}

void logger_initialize(void *s)
{
    logger_t *self = s;
    self->open = &logger_open;
    self->close = &logger_close;
    self->write = &logger_write;
    self->file = NULL;
}