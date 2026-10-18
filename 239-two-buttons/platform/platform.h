#pragma once

#include <stdbool.h>

#define BUTTONS_MAX_NUM 2

// запускает стандартный ввод-вывод и настраивает выводы
void platform_init(void);

void platform_led_set(bool on);
bool platform_led_get(void);

// нажата ли кнопка сейчас, без подавления дребезга
bool platform_button_read(void);

buttons_count 
typedef struct {
	uint32_t id;
	uint32_t pin;	
} platform_button_t;
