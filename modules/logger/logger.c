//
// Created by gemesa on 11/3/22.
//

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>

#include "logger.h"

static unsigned int calc_pri(int facility, int severity)
{
    return facility + severity;
}

static int logger_open(void *s, char *filename, char *modes)
{
    logger_t *self = s;

    if (self->syslog_enabled == true)
    {
        openlog("rgaas", LOG_PID, LOG_DAEMON);
    }

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

static int logger_close(void *s)
{
    logger_t *self = s;
    if (self->syslog_enabled == true)
    {
        closelog();
    }
    int status = fclose(self->file);
    return status;
}

static int logger_write(void *s, char *msg, int facility, int severity)
{
    logger_t *self = s;
    if (self->syslog_enabled == true)
    {
        syslog(LOG_NOTICE, "%s", msg);
    }
    int status = fprintf(self->file, "%d rgaas - %s\n", calc_pri(facility, severity), msg);
    return status;
}

static int logger_flush(void *s)
{
    logger_t *self = s;
    int status = fflush(self->file);
    return status;
}

static void logger_free(void *s)
{
    logger_t *self = s;
    free(self);
}

static void logger_syslog_enable(void*s, bool flag)
{
    logger_t *self = s;
    self->syslog_enabled = flag;
}

static void logger_initialize(void *s)
{
    logger_t *self = s;
    self->open = &logger_open;
    self->close = &logger_close;
    self->write = &logger_write;
    self->flush = &logger_flush;
    self->free = &logger_free;
    self->file = NULL;
    self->enable_syslog = &logger_syslog_enable;
    self->syslog_enabled = false;
}

logger_t *logger_new(void)
{
    logger_t *self = malloc(sizeof(logger_t));
    logger_initialize(self);
    return self;
}
