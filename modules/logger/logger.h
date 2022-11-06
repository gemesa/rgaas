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
    int (*write)(void *s, char *msg, int facility, int severity);
    int (*flush)(void *self);
    void (*free)(void *self);
    FILE *file;
    bool enable_syslog;
} logger_t;

extern logger_t *logger_new(void);

#endif//RGAAS_LOGGER_H
