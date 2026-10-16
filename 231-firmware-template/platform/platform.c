#include "platform.h"

#include "pico/stdlib.h"
#include "hardware/gpio.h"

static const uint BUTTON_PIN = 15;

static void button_init(void)
{
	gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_PIN);
	
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
	button_init();
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
bool platform_button_read(void)
{
	if (!gpio_get(BUTTON_PIN)) 
		return true;
	else 
		return false;
}
