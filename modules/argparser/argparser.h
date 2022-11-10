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
    int argc;
    char **argv;
    int optind;
    unsigned int port_number;
    bool verbose_output;
} args_generic_t;

typedef struct
{
    args_generic_t generic;
    struct
    {
        bool test_mode;
        char *hostname;
    } client;
} args_client_t;

typedef struct
{
    args_generic_t generic;
    struct
    {
        bool syslog_enabled;
        process_mode_t process_mode;
        char *log_file;
    } server;
} args_server_t;

typedef struct
{
    void (*parse)(void *self, int argc, char **argv);
    void (*free)(void *self);
    char *usage_info_generic;
    char *usage_info;
    int status;
    bool non_opt_arg_found;
} argparser_generic_t;

typedef struct
{
    argparser_generic_t generic;
    args_client_t args;
} argparser_client_t;

typedef struct
{
    argparser_generic_t generic;
    args_server_t args;
} argparser_server_t;

extern argparser_client_t *argparser_client_new(void);
extern argparser_server_t *argparser_server_new(void);

#endif//RGAAS_ARGPARSER_H
