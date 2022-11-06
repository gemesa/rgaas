//
// Created by gemesa on 11/3/22.
//

#ifndef RGAAS_LOGGER_H
#define RGAAS_LOGGER_H

#include <stdio.h>

typedef struct
{
    int (*open)(void *self, char *filename, char *modes);
    int (*close)(void *self);
    int (*write)(void *self, char *msg);
    int (*flush)(void *self);
    FILE *file;
} logger_t;

extern logger_t *logger_new(void);

#endif//RGAAS_LOGGER_H
