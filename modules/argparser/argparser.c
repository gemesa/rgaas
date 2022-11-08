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

static char *usage_info = "Usage: rgaas-server -p port_number [-h] [-v] [-d] [-s] [-l log_file]\n"
                          "  p: port number\n"
                          "  h: print help message\n"
                          "  v: enable verbose output\n"
                          "  d: daemon mode (default: process is running in foreground)\n"
                          "  s: enable syslog (default: syslog is disabled)\n"
                          "  l: log file (default: process is logging to stdout, IMPORTANT: if -l is provided the destination directory has to exist)\n"
                          "Note: -s and/or -l should be provided together with -d as stdout and stderr are redirected to /dev/null in daemon mode\n";

void argparser_argparse(void *s, int argc, char **argv)
{
    argparser_t *self = s;
    int c;
    char *ptr;
    while ((c = getopt(argc, argv, "p:vdsl:h")) != -1)
    {
        switch (c)
        {
            case 'p':
                self->args.port_number = strtoul(optarg, &ptr, CONVERSION_BASE);
            case 'v':
                self->args.verbose_output = true;
                break;
            case 'd':
                self->args.process_mode = DAEMON_PROCESS;
                break;
            case 's':
                self->args.syslog_enabled = true;
                break;
            case 'l':
                self->args.log_file = optarg;
                break;
            case 'h':
                self->status = EXIT_FAILURE;
            default:
                self->status = EXIT_FAILURE;
        }
    }

    if (optind < argc)
    {
        self->non_opt_arg_found = true;
    }

    self->args.optind = optind;
    self->args.argc = argc;
    self->args.argv = argv;
}

static void argparser_initialize(void *s)
{
    argparser_t *self = s;
    self->parse = &argparser_argparse;
    self->free = &free;
    self->args.port_number = DEFAULT_PORT_NUMBER;
    self->args.verbose_output = false;
    self->args.process_mode = FOREGROUND_PROCESS;
    self->args.syslog_enabled = false;
    self->args.log_file = NULL;
    self->non_opt_arg_found = false;
    self->status = EXIT_SUCCESS;
    self->usage_info = usage_info;
}

argparser_t *argparser_new(void)
{
    argparser_t *self = malloc(sizeof(argparser_t));
    argparser_initialize(self);
    return self;
}