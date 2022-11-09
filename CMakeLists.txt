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
        modules/logger/logger.c modules/logger/logger.h
        modules/signal_handler/signal_handler.c modules/signal_handler/signal_handler.h
        modules/argparser/argparser.c modules/argparser/argparser.h
        modules/tcp_handler/tcp_handler.c modules/tcp_handler/tcp_handler.h)

add_executable(rgaas-client
        rgaas/client/main.c
        modules/logger/logger.c modules/logger/logger.h
        modules/signal_handler/signal_handler.c modules/signal_handler/signal_handler.h
        modules/argparser/argparser.c modules/argparser/argparser.h)