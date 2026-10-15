cmake_minimum_required(VERSION 3.15)

project(device)

add_library(device STATIC
    src/device.c
)

target_include_directories(device PUBLIC
    include
)

target_link_libraries(device PRIVATE
    pico_stdlib
    pico_unique_id
)

target_compile_definitions(${PROJECT_NAME} PRIVATE
    DEVICE_BOARD="${PICO_BOARD}"
)