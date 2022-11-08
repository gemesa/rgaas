//
// Created by gemesa on 11/3/22.
//

#ifndef RGAAS_LOGGER_H
#define RGAAS_LOGGER_H

#include <stdbool.h>
#include <stdio.h>

typedef struct
{
    int (*open)(void *self, char *filename, char *modes);
    int (*close)(void *self);
    int (*write)(void *s, int facility, int severity, const char *format, ...);
    int (*flush)(void *self);
    void (*free)(void *self);
    FILE *file;
    void (*enable_syslog)(void *self, bool flag);
    bool syslog_enabled;
    void (*enable_verbose_output)(void *self, bool flag);
    bool verbose_output;
} logger_t;

extern logger_t *logger_new(void);

#endif//RGAAS_LOGGER_H
