#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef enum {
    BUTTON_STATE_PRESSED,
    BUTTON_STATE_PRESSING,
	BUTTON_STATE_RELEASED,
    BUTTON_STATE_RELEASING,
} button_state_t;

void button_task_init(void);
void button_task_handle(void);
uint64_t button_task_get_press_count(void);
bool button_task_is_pressed(void);
