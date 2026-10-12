#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef enum {
    BUTTON_STATE_OFF,
    BUTTON_STATE_OFF
	BUTTON_STATE_ON,
    BUTTON_STATE_BLINK,
} led_state_t;

void button_task_init(void);
void button_task_handle(void);