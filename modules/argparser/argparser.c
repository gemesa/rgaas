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

static char *usage_info = "Usage: rgaas-server [-n hostname] [-p port_number] [-h] [-v] [-d] [-s] [-l log_file] [-t]\n"
                          "  n: hostname\n"
                          "     - specify hostname\n"
                          "     - this argument is only used by the client\n"
                          "  p: port number\n"
                          "     - specify port number\n"
                          "  h:\n"
                          "     - print help message\n"
                          "  v:\n"
                          "     - enable verbose output\n"
                          "  d:\n"
                          "     - enable daemon mode\n"
                          "     - default: process is running in foreground)\n"
                          "     - this argument is only used by the server\n"
                          "  s:\n"
                          "     - enable syslog\n"
                          "     - default: syslog is disabled\n"
                          "     - this argument is only used by the server\n"
                          "  l: log_file\n"
                          "     - specify log file\n"
                          "     - default: process is logging to stdout\n"
                          "     - this argument is only used by the server\n"
                          "     - IMPORTANT: if -l is provided the destination directory has to exist)\n"
                          "     - -s and/or -l should be provided together with -d as stdout and stderr are redirected to /dev/null in daemon mode\n"
                          "  t:\n"
                          "     - test mode\n"
                          "         - number of requested bytes is hardcoded to 5 in client\n"
                          "         - number of requests is hardcoded to 1 in client\n"
                          "     - this argument is only used by the client\n";

void argparser_argparse(void *s, int argc, char **argv)
{
    argparser_t *self = s;
    int c;
    char *ptr;
    while ((c = getopt(argc, argv, "n:p:vdsl:ht")) != -1)
    {
        switch (c)
        {
            case 'n':
                self->args.hostname = optarg;
                break;
            case 'p':
                self->args.port_number = strtoul(optarg, &ptr, CONVERSION_BASE);
                break;
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
                break;
            case 't':
                self->args.test_mode = true;
                break;
            default:
                self->status = EXIT_FAILURE;
                break;
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
    self->args.test_mode = false;
    self->args.hostname = "localhost";
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