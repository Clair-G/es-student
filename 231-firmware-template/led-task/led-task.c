#include "led-task.h"

#include "pico/stdlib.h"

// Состояние задачи: внутреннее связывание, только для этого файла
static led_state_t led_state;
static uint64_t half_period_us;
static uint64_t last_toggle_us;
static const uint32_t DEFAULT_LED_BLINK_PERIOD_MS=1000;

bool period_elapsed(uint64_t *last_us, uint64_t period_us);

// Вспомогательная функция
static void led_set(bool on)
{
    gpio_put(PICO_DEFAULT_LED_PIN, on);
}

bool led_task_set_period_ms(uint32_t period_ms)
{
    // С нулевым полупериодом светодиод переключался бы
    // на каждой итерации: такой период не принимается
    if (period_ms == 0) {
        return false;
    }

    half_period_us = (uint64_t)period_ms * 1000 / 2;
    return true;
}

void led_task_init(void)
{
	gpio_init(PICO_DEFAULT_LED_PIN);
	gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
	
	led_task_set_period_ms(DEFAULT_LED_BLINK_PERIOD_MS);
    led_task_set_state(LED_STATE_BLINK);
}

void led_task_set_state(led_state_t state)
{
    led_state = state;
	switch (led_state)
	{
		case LED_STATE_OFF:
		{
			led_set(false);
			break;
		}
		case LED_STATE_ON:
		{
			led_set(true);
			break;
		}
		case LED_STATE_BLINK:
		{
			last_toggle_us = 0;
			break;
		}

		
	//	gpio_put(LED_PIN, led_state);
	}
}

led_state_t led_task_get_state(void)
{
	return led_state;
}

void led_task_handle(void)
{
    switch (led_state)
    {	// «выключен» и «горит» — ничего не делаем, светодиод установлен при входе
		case LED_STATE_OFF:
		case LED_STATE_ON:
			break;
	    // «мигает» — каждые полпериода переключаем светодиод
		case LED_STATE_BLINK:
			if (period_elapsed(&last_toggle_us, half_period_us)) 
			{
				gpio_put(PICO_DEFAULT_LED_PIN, !gpio_get(PICO_DEFAULT_LED_PIN));
			}
			break;
    }
}

uint32_t led_task_get_period_ms(void)
{
	return (uint32_t)(half_period_us / 500);
}

bool period_elapsed(uint64_t *last_us, uint64_t period_us)
{
    uint64_t now_us = time_us_64();

    if (now_us - *last_us < period_us) {
        return false;
    }

    // Следующее срабатывание отсчитывается от прошлого, а не от «сейчас»
    *last_us += period_us;

    // Отстали больше чем на период: не догонять, а начать отсчёт заново
    if (now_us - *last_us >= period_us) {
        *last_us = now_us;
    }

    return true;
}