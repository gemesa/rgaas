//
// Created by gemesa on 11/3/22.
//

#ifndef RGAAS_LOGGER_H
#define RGAAS_LOGGER_H

#include <stdio.h>

typedef struct
{
    char *start;
    char* start_logger_failed;
    char *terminate;
} messages_t;

typedef struct
{
    int (*open)(void *self, char *filename, char *modes);
    int (*close)(void *self);
    int (*write)(void *self, char *msg);
    FILE *file;
} logger_t;

extern const messages_t messages;
extern void logger_initialize(void *s);

#endif//RGAAS_LOGGER_H
