//
// Created by gemesa on 11/3/22.
//

#include "argparser.h"

#include <stdlib.h>
#include <unistd.h>

enum
{
    CONVERSION_BASE = 10,
    DEFAULT_PORT_NUMBER = 8000
};

static char *usage_info_generic = "  p: port number\n"
                                  "     - specify port number\n"
                                  "  h:\n"
                                  "     - print help message\n"
                                  "  v:\n"
                                  "     - enable verbose output\n";


static char *usage_info_client = "Usage: rgaas-client [-n hostname] [-p port_number] [-h] [-v] [-t]\n"
                                 "  n: hostname\n"
                                 "     - specify hostname\n"
                                 "  t:\n"
                                 "     - test mode\n"
                                 "         - number of requested bytes is hardcoded to 5\n"
                                 "         - number of requests is hardcoded to 1\n";

static char *usage_info_server = "Usage: rgaas-server [-p port_number] [-h] [-v] [-d] [-s] [-l log_file]\n"
                                 "  d:\n"
                                 "     - enable daemon mode\n"
                                 "     - default: process is running in foreground)\n"
                                 "  s:\n"
                                 "     - enable syslog\n"
                                 "     - default: syslog is disabled\n"
                                 "  l: log_file\n"
                                 "     - specify log file\n"
                                 "     - default: process is logging to stdout\n"
                                 "     - IMPORTANT: if -l is provided the destination directory has to exist)\n"
                                 "     - -s and/or -l should be provided together with -d as stdout and stderr are redirected to /dev/null in daemon mode\n";

void argparser_argparse_client(void *s, int argc, char **argv)
{
    argparser_client_t *self = s;
    int c;
    char *ptr;
    while ((c = getopt(argc, argv, "n:p:vht")) != -1)
    {
        switch (c)
        {
            case 'n':
                self->args.client.hostname = optarg;
                break;
            case 'p':
                self->args.generic.port_number = strtoul(optarg, &ptr, CONVERSION_BASE);
                break;
            case 'v':
                self->args.generic.verbose_output = true;
                break;
            case 'h':
                self->generic.status = EXIT_FAILURE;
                break;
            case 't':
                self->args.client.test_mode = true;
                break;
            default:
                self->generic.status = EXIT_FAILURE;
                break;
        }
    }

    if (optind < argc)
    {
        self->generic.non_opt_arg_found = true;
    }

    self->args.generic.optind = optind;
    self->args.generic.argc = argc;
    self->args.generic.argv = argv;
}

void argparser_argparse_server(void *s, int argc, char **argv)
{
    argparser_server_t *self = s;
    int c;
    char *ptr;
    while ((c = getopt(argc, argv, "p:vdsl:h")) != -1)
    {
        switch (c)
        {
            case 'p':
                self->args.generic.port_number = strtoul(optarg, &ptr, CONVERSION_BASE);
                break;
            case 'v':
                self->args.generic.verbose_output = true;
                break;
            case 'd':
                self->args.server.process_mode = DAEMON_PROCESS;
                break;
            case 's':
                self->args.server.syslog_enabled = true;
                break;
            case 'l':
                self->args.server.log_file = optarg;
                break;
            case 'h':
                self->generic.status = EXIT_FAILURE;
                break;
            default:
                self->generic.status = EXIT_FAILURE;
                break;
        }
    }

    if (optind < argc)
    {
        self->generic.non_opt_arg_found = true;
    }

    self->args.generic.optind = optind;
    self->args.generic.argc = argc;
    self->args.generic.argv = argv;
}

static void argparser_initialize_generic(void *s)
{
    argparser_generic_t *self = s;
    self->free = &free;
    self->usage_info_generic = usage_info_generic;
    self->non_opt_arg_found = false;
    self->status = EXIT_SUCCESS;
}

static void argparser_client_initialize(void *s)
{
    argparser_client_t *self = s;
    self->generic.parse = &argparser_argparse_client;
    self->generic.usage_info = usage_info_client;
    self->args.generic.port_number = DEFAULT_PORT_NUMBER;
    self->args.generic.verbose_output = false;
    self->args.client.test_mode = false;
    self->args.client.hostname = "localhost";
}

static void argparser_server_initialize(void *s)
{
    argparser_server_t *self = s;
    self->generic.parse = &argparser_argparse_server;
    self->generic.usage_info = usage_info_server;
    self->args.generic.port_number = DEFAULT_PORT_NUMBER;
    self->args.generic.verbose_output = false;
    self->args.server.process_mode = FOREGROUND_PROCESS;
    self->args.server.syslog_enabled = false;
    self->args.server.log_file = NULL;

}

argparser_client_t *argparser_client_new(void)
{
    argparser_client_t *self = malloc(sizeof(argparser_client_t));
    argparser_initialize_generic(self);
    argparser_client_initialize(self);
    return self;
}

argparser_server_t *argparser_server_new(void)
{
    argparser_server_t *self = malloc(sizeof(argparser_server_t));
    argparser_initialize_generic(self);
    argparser_server_initialize(self);
    return self;
}