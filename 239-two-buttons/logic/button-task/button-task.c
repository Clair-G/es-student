#include "button-task/button-task.h"


#include "systime/systime.h"
#include "logging/log.h"

#include "platform/platform.h"


typedef enum {
    BUTTON_STATE_PRESSED,
    BUTTON_STATE_PRESSING,
	BUTTON_STATE_RELEASED,
    BUTTON_STATE_RELEASING,
} button_state_t;

static button_state_t button_state;

static const uint64_t DEBOUNCE_US = 20000;

static uint64_t last_pressed_us;
static uint64_t presses;

static button_task_callback_t on_presse_cb;

void button_task_init(button_task_callback_t on_press)
{
	button_state = BUTTON_STATE_RELEASED;
	if (!on_press)
	{
		LOG_ERR("button_task_callback NULL pointer\n");
		return;
	}
	on_presse_cb = on_press;
	
}

void button_task_handle(void)
{
    // читаем уровень: нажатая кнопка замыкает вывод на землю и даёт ноль
    // читаем часы
	
	bool button_pressed = platform_button_read(); 
	uint64_t now_us = systime_us();
	
    switch (button_state)
    {
    // переходы по таблице; при переходе в «нажата»
    // увеличиваем счётчик и вызываем led_task_next_state()
	
		case BUTTON_STATE_RELEASED:
		{
			if (button_pressed)
			{
				last_pressed_us = now_us; 
				button_state = BUTTON_STATE_PRESSING;
			}
			break;
		}
		case BUTTON_STATE_PRESSING:
		{
			if (systime_period_elapsed(&last_pressed_us, DEBOUNCE_US))
			{
				if (button_pressed)
				{
					button_state = BUTTON_STATE_PRESSED;
					presses++;
					//led_task_next_state();
					on_presse_cb();
				}
				else 
					button_state = BUTTON_STATE_RELEASED;
			}	
			break;
			
		}
		case BUTTON_STATE_PRESSED:
		{
			if (!button_pressed)
			{
				last_pressed_us = now_us;
				button_state = BUTTON_STATE_RELEASING;				
			}
			break;
		}
		case BUTTON_STATE_RELEASING:
		{
			if (systime_period_elapsed(&last_pressed_us, DEBOUNCE_US))
			{
				if (!button_pressed)
					button_state = BUTTON_STATE_PRESSED;
				else 
					button_state = BUTTON_STATE_RELEASED;
			}
			break;
		}
		
    }
}

/*
uint64_t button_task_get_press_count(void)
{
	return presses;	
}

bool button_task_is_pressed(void)
{
	return platform_button_read();
}
*/

bool button_task_get_info(unint32_t button_id, button_info_t *info)
{
	if (id >= stopwatches_num)
		return false;
	
	if (platform_button_read(id)) 
		info->state = "pressed";
	else info->state = "relesed";
	info.presses = buttons[id].presses;
		
	return true;
}