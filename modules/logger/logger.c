//
// Created by gemesa on 11/3/22.
//

#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "logger.h"

enum
{
    HOSTNAME_SIZE = 50
};

static unsigned int calc_pri(int facility, int severity)
{
    return facility + severity;
}

static void get_time(char *time_formatted)
{
    time_t time_raw;
    (void) time(&time_raw);
    char time_tmp[sizeof "2022-11-02T08:00:00+01:00"];
    (void) strftime(time_tmp, sizeof time_tmp, "%FT%T+01:00", gmtime(&time_raw));
    (void) strncpy(time_formatted, time_tmp, sizeof(time_tmp));
}

static void get_h_name(char *h_name)
{
    char hostname[HOSTNAME_SIZE] = {[0 ... HOSTNAME_SIZE - 1] = '\0'};
    gethostname(hostname, HOSTNAME_SIZE - 1);
    struct hostent *h;
    h = gethostbyname(hostname);
    (void) strncpy(h_name, h->h_name, HOSTNAME_SIZE);
}

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

static int logger_close(void *s)
{
    logger_t *self = s;
    int status = fclose(self->file);
    return status;
}

static int logger_write(void *s, char *msg, int facility, int severity)
{
    logger_t *self = s;
    char time[sizeof "2022-11-02T08:00:00+01:00"];
    char h_name[HOSTNAME_SIZE];

    get_time(time);
    get_h_name(h_name);
    int status = fprintf(self->file, "%d %s %s rgaas - - - %s\n", calc_pri(facility, severity), time, h_name, msg);
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

static void logger_initialize(void *s)
{
    logger_t *self = s;
    self->open = &logger_open;
    self->close = &logger_close;
    self->write = &logger_write;
    self->flush = &logger_flush;
    self->free = &logger_free;
    self->file = NULL;
}

logger_t *logger_new(void)
{
    logger_t *self = malloc(sizeof(logger_t));
    logger_initialize(self);
    return self;
}
