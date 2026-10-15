cmake_minimum_required(VERSION 3.15)

project(systime)

add_library(systime STATIC
    src/systime.c
)

target_include_directories(systime PUBLIC
    include
)

target_link_libraries(systime PRIVATE
    pico_stdlib
)
