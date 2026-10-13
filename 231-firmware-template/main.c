#include <stdio.h>
#include <string.h>

#include "led-task/led-task.h"
#include "button-task/button-task.h"
#include "pi-task/pi-task.h"
#include "log.h"
#include "device.h"
#include "profiling.h"
#include "stdio-text-protocol.h"
#include "api.h"

#include "pico/stdlib.h"

#define LINE_SIZE 32


// прикидка: за член ряда 4 операции с double, 175 + 110 + 190 + 110 = 585 тактов;
// 1 000 000 членов по 585 тактов при 125 МГц — около 4,7 с
//const uint CALC_PI_TERMS = 1000000;

extern volatile double pi_result;

//-----------------

int main()
{
	stdio_init_all();
	profiling_init();
	stdio_text_protocol_init();
	led_task_init();
	button_task_init();
	pi_task_init();

	
    while (1)
    {
        profiling_iteration();
		led_task_handle();
		button_task_handle();
		pi_task_handle();
	    
		//read_line();
		
		// принятая строка становится командой, команда — действием прибора
		const command_t *command = stdio_text_protocol_handle();
		if (command != NULL)
		{
			api_handle(command);
		}
  	
    }
	
	
}