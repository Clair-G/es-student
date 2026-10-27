#pragma once

#include <stdbool.h>
#include <stdint.h>

#define BUTTONS_MAX_NUM 2

// запускает стандартный ввод-вывод и настраивает выводы
void platform_init(void);

void platform_led_set(bool on);
bool platform_led_get(void);

// нажата ли кнопка сейчас, без подавления дребезга
bool platform_button_read(uint32_t button_id);

typedef struct {
	uint32_t pin;	
} platform_button_t;

//extern const uint32_t buttons_count;

float platform_voltage_read(void);
float platform_temp_read(void);