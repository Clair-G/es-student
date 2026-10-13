#pragma once

#include <stdbool.h>
#include <stdint.h>

void button_task_init(void);
void button_task_handle(void);
uint64_t button_task_get_press_count(void);
bool button_task_is_pressed(void);
