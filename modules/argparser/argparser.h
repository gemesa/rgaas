//
// Created by gemesa on 11/3/22.
//

#ifndef RGAAS_ARGPARSER_H
#define RGAAS_ARGPARSER_H

#include <stdbool.h>

typedef enum
{
    FOREGROUND_PROCESS,
    DAEMON_PROCESS
} process_mode_t;

typedef struct
{
    process_mode_t process_mode;
    bool syslog_enabled;
    char *log_file;
} args_t;

typedef struct
{
    void (*parse)(void *self, int argc, char **argv);
    void (*free)(void *self);
    args_t args;
    int status;
    bool non_opt_arg_found;
    char *usage_info;
} argparser_t;

extern argparser_t *argparser_new(void);

#endif//RGAAS_ARGPARSER_H
