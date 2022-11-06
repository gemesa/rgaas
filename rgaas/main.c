//
// Created by gemesa on 11/2/22.
//

#include <stdlib.h>

#include "../modules/logger/logger.h"

int main(void)
{
    logger_t *logger = logger_new();

    int status = logger->open(logger, NULL, "a+");

    if (status == EXIT_SUCCESS)
    {
        logger->write(logger, "program started", USER, NOTICE);
    }
    else
    {
        logger->write(logger, "program started (logger initialization failed, logging to stdout)", USER, ERROR);
    }

    logger->write(logger, "program terminating...", USER, NOTICE);
    logger->close(logger);
    logger->free(logger);

    return 0;
}
