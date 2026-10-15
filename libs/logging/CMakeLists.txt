cmake_minimum_required(VERSION 3.15)

project(logging)

add_library(logging STATIC
    src/log.c
)

target_include_directories(logging PUBLIC
    include
)

