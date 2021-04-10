cmake_minimum_required(VERSION 3.17)
project(cproxy C)

set(CMAKE_C_STANDARD 11)

include_directories(src)

add_executable(cproxy
        src/cproxy.c
        src/cproxy.h
        src/consumer.c
        src/producer.c)
