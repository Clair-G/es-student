#include "button-task.h"
#include "led-task/led-task.h"
#include "pico/stdlib.h"
#include "hardware/gpio.h"

static button_state_t button_state;

static const uint64_t DEBOUNCE_US = 20000;
static const uint BUTTON_PIN = 15;
static uint64_t last_pressed_us;
static uint64_t presses;

void button_task_init(void)
{
	gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_PIN);
	button_state = BUTTON_STATE_RELEASED;
}

static bool period_elapsed(uint64_t *last_us, uint64_t period_us)
{
    uint64_t now_us = time_us_64();

    if (now_us - *last_us <= period_us) 
	{
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


void button_task_handle(void)
{
    // читаем уровень: нажатая кнопка замыкает вывод на землю и даёт ноль
    // читаем часы
	
	bool pin_state = gpio_get(BUTTON_PIN); 
	uint64_t now_us = time_us_64();
	
    switch (button_state)
    {
    // переходы по таблице; при переходе в «нажата»
    // увеличиваем счётчик и вызываем led_task_next_state()
	
		case BUTTON_STATE_RELEASED:
		{
			if (!pin_state)
			{
				last_pressed_us = now_us; 
				button_state = BUTTON_STATE_PRESSING;
			}
			break;
		}
		case BUTTON_STATE_PRESSING:
		{
			if (period_elapsed(&last_pressed_us, DEBOUNCE_US))
			{
				if (!pin_state)
				{
					button_state = BUTTON_STATE_PRESSED;
					presses++;
					led_task_next_state();
				}
				else 
					button_state = BUTTON_STATE_RELEASED;
			}	
			break;
			
		}
		case BUTTON_STATE_PRESSED:
		{
			if (pin_state)
			{
				last_pressed_us = now_us;
				button_state = BUTTON_STATE_RELEASING;				
			}
			break;
		}
		case BUTTON_STATE_RELEASING:
		{
			if (period_elapsed(&last_pressed_us, DEBOUNCE_US))
			{
				if (pin_state)
					button_state = BUTTON_STATE_PRESSED;
				else 
					button_state = BUTTON_STATE_RELEASED;
			}
			break;
		}
		
    }
}


bool button_task_is_pressed(void)
{
	if (!gpio_get(BUTTON_PIN)) 
		return true;
	else 
		return false;
}

uint64_t button_task_get_press_count(void)
{
	return presses;	
}