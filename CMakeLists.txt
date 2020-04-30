cmake_minimum_required(VERSION 3.16)
project(svc C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(svc
        svc.c
        svc.h
        tester.c)
