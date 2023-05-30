//
// Created by gemesa on 11/3/22.
//

#include <stdarg.h>
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

static int logger_write(void *s, int facility, int severity, const char *format, ...)
{
    logger_t *self = s;

    va_list args;
    va_start(args, format);

    if (self->syslog_enabled == true)
    {
        vsyslog(LOG_NOTICE, format, args);
    }
    int status = EXIT_SUCCESS;
    if (self->verbose_output == true)
    {
        if (fprintf(self->file, "%d rgaas - ", calc_pri(facility, severity)) < 0)
        {
            status = EXIT_FAILURE;
        }
        if (vfprintf(self->file, format, args) < 0)
        {
            status = EXIT_FAILURE;
        }
    }
    else
    {
        if (severity < LOG_DEBUG)
        {
            if (vfprintf(self->file, format, args) < 0)
            {
                status = EXIT_FAILURE;
            }
        }
    }
    self->flush(self);

    va_end(args);

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

static void logger_syslog_enable(void *s, bool flag)
{
    logger_t *self = s;
    self->syslog_enabled = flag;
}

static void logger_enable_verbose_output(void *s, bool flag)
{
    logger_t *self = s;
    self->verbose_output = flag;
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
    self->enable_verbose_output = &logger_enable_verbose_output;
    self->verbose_output = false;
}

logger_t *logger_new(void)
{
    logger_t *self = malloc(sizeof(logger_t));
    logger_initialize(self);
    return self;
}
