//
// Created by gemesa on 11/3/22.
//

#include "argparser.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

args_t argparse(int argc, char **argv)
{
    args_t args = {.process_mode = FOREGROUND_PROCESS, .syslog_enabled = false, .log_file = NULL};
    char *usage_info = "Usage: %s [-d] [-s] [-l log_file]\n"
                       "  d: daemon mode (default: process is running in foreground)\n"
                       "  s: enable syslog (default: syslog is disabled)\n"
                       "  l: log file (default: process is logging to stdout, IMPORTANT: if -l is provided the destination directory has to exist)\n"
                       "Note: -s and/or -l should be provided together with -d as stdout and stderr are redirected to /dev/null in daemon mode\n";

    int c;
    while ((c = getopt(argc, argv, "dsl:h")) != -1)
    {
        switch (c)
        {
            case 'd':
                args.process_mode = DAEMON_PROCESS;
                break;
            case 's':
                args.syslog_enabled = true;
                break;
            case 'l':
                args.log_file = optarg;
                break;
            case 'h':
                (void) fprintf(stdout, usage_info, argv[0]);
                exit(EXIT_SUCCESS);
            default:
                (void) fprintf(stderr, usage_info, argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    for (int index = optind; index < argc; index++)
    {
        printf("Non-option argument %s\n", argv[index]);
    }

    return args;
}