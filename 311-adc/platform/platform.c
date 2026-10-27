#include "platform.h"

#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"

static const uint STATE_BUTTON_PIN = 15;
#define ADC_PIN 26
#define ADC_CHANNEL 0
#define ADC_TEMP_CHANNEL 4
#define ADC_VREF_V 3.3f
#define ADC_LEVELS 4096

const platform_button_t buttons[] = {
    { STATE_BUTTON_PIN },
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
	adc_init();
	adc_gpio_init(ADC_PIN);
	adc_set_temp_sensor_enabled(true);
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

float platform_voltage_read(void)
{
	adc_select_input(ADC_CHANNEL);
	
	return (float)adc_read() * ADC_VREF_V / ADC_LEVELS;
}

float platform_temp_read(void)
{
	adc_select_input(ADC_TEMP_CHANNEL);
	
	float temp_V = adc_read() * ADC_VREF_V / ADC_LEVELS;
	
	return (float)27.0f - (temp_V - 0.706f) / 0.001721f;
}
