cmake_minimum_required(VERSION 3.15)

set(PICO_BOARD pico)
set(PICO_PLATFORM rp2040)

include(pico_sdk_import.cmake)

project(239-two-buttons)

pico_sdk_init()

add_compile_options(-Wall)

# libs лежит вне папки проекта: второй аргумент задаёт каталог сборки
add_subdirectory(../libs ${CMAKE_BINARY_DIR}/libs)

# выключатель профилирования: 0 — вызовов секундомеров в образе нет
target_compile_definitions(profiling PUBLIC
    PROFILING_ENABLED=1
)

# железо прибора: единственное место проекта с SDK
add_library(platform STATIC
    platform/platform.c
)

# путь к заголовкам — папка проекта, platform подключает pico_stdlib как PRIVATE;
# через этот путь logic находит и firmware.h
target_include_directories(platform PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}/platform
)

target_link_libraries(platform PRIVATE
    pico_stdlib
)

# логика прибора: задачи и команды, без SDK
add_library(logic STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/logic/api/api.c
	${CMAKE_CURRENT_SOURCE_DIR}/logic/api/api-callbacks.c
	${CMAKE_CURRENT_SOURCE_DIR}/logic/led-task/led-task.c
	${CMAKE_CURRENT_SOURCE_DIR}/logic/button-task/button-task.c
	${CMAKE_CURRENT_SOURCE_DIR}/logic/pi-task/pi-task.c
)

# путь к заголовкам — папка logic
target_include_directories(logic PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}/logic
)

# stdio_text_protocol — PUBLIC, остальные библиотеки — PRIVATE
target_link_libraries(logic PUBLIC
    stdio-text-protocol 
)

target_link_libraries(logic PRIVATE
    device
	logging
	profiling
	systime
)


add_executable(${PROJECT_NAME}
	${CMAKE_CURRENT_SOURCE_DIR}/main.c

)

target_include_directories(${PROJECT_NAME} PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(${PROJECT_NAME}
	pico_stdlib
	device
	logging
	profiling
	stdio-text-protocol
	systime
	platform
	logic
)

pico_enable_stdio_usb(${PROJECT_NAME} 1)
pico_enable_stdio_uart(${PROJECT_NAME} 0)

pico_add_extra_outputs(${PROJECT_NAME})