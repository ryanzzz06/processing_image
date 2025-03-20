cmake_minimum_required(VERSION 3.30)
project(processing_image C)

set(CMAKE_C_STANDARD 99)

add_executable(processing_image main.c
        bmp8.c
        bmp8.h
        bmp24.c
        bmp24.h)
