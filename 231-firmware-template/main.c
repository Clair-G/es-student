#include <stdio.h>
#include <string.h>

#include "led.h"
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

const uint BLINK_HALF_PERIOD_MS = 500;

uint64_t last_toggle_us = 0;

extern volatile double pi_result;


void blink(void)
{
    uint64_t now_us = time_us_64();

    if (now_us - last_toggle_us >= BLINK_HALF_PERIOD_MS * 1000)
    {
        last_toggle_us = now_us;
        led_toggle();
    }
}




//-----------------

int main()
{
	stdio_init_all();
	led_init();
	profiling_init();
	stdio_text_protocol_init();
	
    while (1)
    {
        profiling_iteration();
		blink();
	    
		//read_line();
		
		// принятая строка становится командой, команда — действием прибора
		const command_t *command = stdio_text_protocol_handle();
		if (command != NULL)
		{
			api_handle(command);
		}
  	
    }
	
	
}