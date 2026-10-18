#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef void (*button_task_callback_t)(void);

void button_task_init(button_task_callback_t on_press);

void button_task_handle(void);
uint64_t button_task_get_press_count(void);
bool button_task_is_pressed(void);


//Для выдачи в button
typedef struct {
	uint64_t presses;
	const char *state;
} button_info_t;

bool button_task_get_info(unint32_t button_id, button_info_t *info);




