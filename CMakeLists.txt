cmake_minimum_required(VERSION 3.23)
project(rgaas C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

SET (CMAKE_C_COMPILER "/usr/bin/clang")
SET (CMAKE_CXX_COMPILER "/usr/bin/clang++")

file(MAKE_DIRECTORY build)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/build)

add_executable(rgaas-server
        rgaas/server/main.c

        modules/logger/logger.c
        modules/logger/logger.h
        modules/signal_handler/signal_handler.c
        modules/signal_handler/signal_handler.h
        modules/argparser/argparser.c
        modules/argparser/argparser.h
        modules/tcp_handler/tcp_handler.c
        modules/tcp_handler/tcp_handler.h)

add_executable(rgaas-client
        rgaas/client/main.c

        modules/logger/logger.c
        modules/logger/logger.h
        modules/signal_handler/signal_handler.c
        modules/signal_handler/signal_handler.h
        modules/argparser/argparser.c
        modules/argparser/argparser.h
        modules/tcp_handler/tcp_handler.c
        modules/tcp_handler/tcp_handler.h)

add_executable(unit-test
        test/unit_test/cmocka/src/cmocka.c
        test/unit_test/cmocka/include/cmocka.h
        test/unit_test/cmocka/include/cmocka_pbc.h
        test/unit_test/cmocka/include/cmocka_private.h
        test/unit_test/cmocka/include/cmockery/cmockery.h
        test/unit_test/cmocka/include/cmockery/pbc.h

        test/unit_test/unit_test.c
        test/unit_test/argparser_unit_test.c
        test/unit_test/argparser_unit_test.h
        test/unit_test/logger_unit_test.c
        test/unit_test/logger_unit_test.h
        test/unit_test/tcp_handler_unit_test.c
        test/unit_test/tcp_handler_unit_test.h

        modules/argparser/argparser.c
        modules/argparser/argparser.h
        modules/logger/logger.c
        modules/logger/logger.h
        modules/tcp_handler/tcp_handler.c
        modules/tcp_handler/tcp_handler.h
        modules/signal_handler/signal_handler.c
        modules/signal_handler/signal_handler.h)
