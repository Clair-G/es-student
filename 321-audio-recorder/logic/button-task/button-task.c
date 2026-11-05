#include "button-task/button-task.h"


#include "systime/systime.h"
#include "logging/log.h"

#include "platform/platform.h"

extern const uint32_t buttons_count;

#define MAX_BUTTONS_NUMBER 3

typedef enum {
    BUTTON_STATE_PRESSED,
    BUTTON_STATE_PRESSING,
	BUTTON_STATE_RELEASED,
    BUTTON_STATE_RELEASING,
} button_state_t;

typedef struct {
	uint64_t presses;	
	uint64_t last_pressed_us;
	uint64_t press_started_us;
	button_state_t state;
	button_task_callback_t on_press_cb;
	button_task_callback_t on_long_press_cb;
	bool long_press_enabled;
	bool reported;
} button_t;

static button_t buttons[MAX_BUTTONS_NUMBER]; //кол-во кнопок - берем из платформы

static const uint64_t DEBOUNCE_US = 20000;
static const uint64_t LONG_PRESS_US = 1000000;

//static uint64_t last_pressed_us;
//static uint64_t presses;

//static button_task_callback_t on_presse_cb;

void button_task_init(uint32_t button_id, button_task_callback_t on_press, button_task_callback_t on_long_press)
{
	buttons[button_id].state = BUTTON_STATE_RELEASED;
	if (!on_press)
	{
		LOG_ERR("button_task_callback NULL pointer\n");
		return;
	}
	buttons[button_id].on_press_cb = on_press;
	if (on_long_press != NULL)
	{	
		buttons[button_id].on_long_press_cb = on_long_press;
		buttons[button_id].long_press_enabled = true;
	}
	else
		buttons[button_id].long_press_enabled = false;
	buttons[button_id].reported = false;

}

void button_task_handle(void) //Для одинаковых кнопок, в цикле
{
    // читаем уровень: нажатая кнопка замыкает вывод на землю и даёт ноль
    // читаем часы
	for (int button_id = 0; button_id < buttons_count; button_id++) 
	{ 
		bool button_pressed = platform_button_read(button_id); 
		uint64_t now_us = systime_us();
		
		switch (buttons[button_id].state)
		{
		// переходы по таблице; при переходе в «нажата»

		
			case BUTTON_STATE_RELEASED:
			{
				if (button_pressed)
				{
					buttons[button_id].last_pressed_us = now_us; 
					buttons[button_id].state = BUTTON_STATE_PRESSING;
				}
				break;
			}
			case BUTTON_STATE_PRESSING:
			{
				if (systime_period_elapsed(&buttons[button_id].last_pressed_us, DEBOUNCE_US))
				{
					if (button_pressed)
					{
						buttons[button_id].state = BUTTON_STATE_PRESSED;
						buttons[button_id].presses++;
						buttons[button_id].press_started_us = systime_us();
						
						if (buttons[button_id].long_press_enabled == false) //Если короткое - выполняем
						{
							buttons[button_id].on_press_cb();
							buttons[button_id].reported = true;
						}
					}
					else 
						buttons[button_id].state = BUTTON_STATE_RELEASED;
				}	
				break;
				
			}
			case BUTTON_STATE_PRESSED:
			{
				if (buttons[button_id].long_press_enabled)
				{	
					if (systime_period_elapsed(&buttons[button_id].press_started_us, LONG_PRESS_US) 
						&& buttons[button_id].reported == false)
					{
						buttons[button_id].on_long_press_cb();
						buttons[button_id].reported = true;	
						
					}
				}
			
				if (!button_pressed)
					{
						buttons[button_id].last_pressed_us = now_us;
						buttons[button_id].state = BUTTON_STATE_RELEASING;				
					}
					break;
			}
			case BUTTON_STATE_RELEASING:
			{
				if (systime_period_elapsed(&buttons[button_id].last_pressed_us, DEBOUNCE_US))
				{
					if (!button_pressed)
					{
						buttons[button_id].state = BUTTON_STATE_RELEASED;
						if (buttons[button_id].reported == false)
						{
							buttons[button_id].on_press_cb();
							
						}
						else
							buttons[button_id].reported = false;
					}
					else 
					{
						buttons[button_id].state = BUTTON_STATE_PRESSED;
					}
				}
				break;
			}
			
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

bool button_task_get_info(uint32_t button_id, button_info_t *info)
{
	if (button_id >= buttons_count)
		return false;
	
	info->state = platform_button_read(button_id); 
	info->presses = buttons[button_id].presses;
	return true;
}