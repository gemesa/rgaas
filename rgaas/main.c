//
// Created by gemesa on 11/2/22.
//

#include <stdlib.h>

#include "../modules/logger/logger.h"

int main(void)
{
    logger_t logger;

    logger_initialize(&logger);
    int status = logger.open(&logger, NULL, "a+");

    if (status == EXIT_SUCCESS)
    {
        logger.write(&logger, messages.start);
    }
    else
    {
        logger.write(&logger, messages.start_logger_failed);
    }

    logger.write(&logger, messages.terminate);
    logger.close(&logger);

    return 0;
}
