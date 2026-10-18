#include "platform.h"

#include "pico/stdlib.h"
#include "hardware/gpio.h"

static const uint STATE_BUTTON_PIN = 15;
static const uint PERIOD_BUTTON_PIN = 14;

const platform_button_t buttons[] = {
    { STATE_BUTTON_PIN },
	{ PERIOD_BUTTON_PIN },
};

const uint32_t buttons_count = sizeof(buttons) / sizeof(buttons[0]);

static void buttons_init(void)
{
	for (int button_id = 0; button_id < buttons_count; button_id++)
	{
		gpio_init(buttons[button_id].pin);
		gpio_set_dir(buttons[button_id].pin, GPIO_IN);
		gpio_pull_up(buttons[button_id].pin);
	}
}

static void led_init(void)
{
	gpio_init(PICO_DEFAULT_LED_PIN);
	gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
	

}

// запускает стандартный ввод-вывод и настраивает выводы
void platform_init(void)
{
	stdio_init_all();
	buttons_init();
	led_init();
	
}


void platform_led_set(bool on)
{
	gpio_put(PICO_DEFAULT_LED_PIN, on);
		
}

bool platform_led_get(void)
{
	return gpio_get(PICO_DEFAULT_LED_PIN);
		
}

// нажата ли кнопка сейчас, без подавления дребезга
bool platform_button_read(uint32_t button_id) //
{
	if (!gpio_get(buttons[button_id].pin)) 
		return true;
	
	return false;
}

