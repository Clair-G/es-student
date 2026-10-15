cmake_minimum_required(VERSION 3.15)

project(profiling)

add_library(profiling STATIC
    src/profiling.c
)

target_include_directories(profiling PUBLIC
    include
)

target_link_libraries(profiling PRIVATE
    pico_stdlib
)
