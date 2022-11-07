//
// Created by gemesa on 11/3/22.
//

#ifndef RGAAS_ARGPARSER_H
#define RGAAS_ARGPARSER_H

#include <stdbool.h>

typedef struct
{
    bool syslog_enabled;
    char* log_file;
} args_t;

extern args_t argparse(int argc, char **argv);

#endif //RGAAS_ARGPARSER_H
