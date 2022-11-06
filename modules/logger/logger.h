//
// Created by gemesa on 11/3/22.
//

#ifndef RGAAS_LOGGER_H
#define RGAAS_LOGGER_H

#include <stdio.h>

typedef enum
{
    KERN,
    USER,
    MAIL,
    DAEMON,
    AUTH,
    SYSLOG,
    LPR,
    NEWS,
    UUCP,
    CRON,
    AUTHPRIV,
    FTP,
    NTP,
    SECURITY,
    CONSOLE,
    SOLARIS_CRON,
    LOCAL0,
    LOCAL1,
    LOCAL2,
    LOCAL3,
    LOCAL4,
    LOCAL5,
    LOCAL6,
    LOCAL7
} facility_t;

typedef enum
{
    EMERGENCY,
    ALERT,
    CRITICAL,
    ERROR,
    WARNING,
    NOTICE,
    INFORMATIONAL,
    DEBUG
} severity_t;

typedef struct
{
    int (*open)(void *self, char *filename, char *modes);
    int (*close)(void *self);
    int (*write)(void *s, char *msg, facility_t facility, severity_t severity);
    int (*flush)(void *self);
    void (*free)(void *self);
    FILE *file;
} logger_t;

extern logger_t *logger_new(void);

#endif//RGAAS_LOGGER_H
