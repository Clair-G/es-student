#include <stdio.h>
#include <string.h>

#include "led.h"
#include "log.h"
#include "device.h"
#include "memory.h"
#include "command.h"
#include "clock.h"
#include "profiling.h"

#include "pico/stdlib.h"

#define LOG_LEVEL LOG_LEVEL_ERR

#define LINE_SIZE 32

char line[LINE_SIZE];
uint line_length = 0;

// прикидка: за член ряда 4 операции с double, 175 + 110 + 190 + 110 = 585 тактов;
// 1 000 000 членов по 585 тактов при 125 МГц — около 4,7 с
const uint CALC_PI_TERMS = 1000000;

const uint BLINK_HALF_PERIOD_MS = 500;

uint64_t last_toggle_us = 0;

volatile double pi_result;

void cmd_info(void);
void cmd_version(void);
void cmd_ping(void);
void cmd_version(void);
void cmd_mem_info(void);
void cmd_fw_info(void);
void cmd_dev_info(void);
void cmd_boot_info(void);
void cmd_clk_info(void);
void cmd_uptime(void);
void cmd_calc_pi(void);
void cmd_main_time_exec(void);
void cmd_main_time_reset(void);

const struct command_t commands[] = {
    { "info", cmd_info },
    { "version", cmd_version },
    { "ping", cmd_ping },
	{ "mem_info", cmd_mem_info },
	{ "fw_info", cmd_fw_info },
	{ "dev_info", cmd_dev_info },
	{ "boot_info", cmd_boot_info },
	{ "clk_info", cmd_clk_info },
	{ "uptime", cmd_uptime },
	{ "calc_pi", cmd_calc_pi },
	{ "main_time_exec", cmd_main_time_exec },
	{ "main_time_reset", cmd_main_time_reset },
};

const uint command_count = sizeof(commands) / sizeof(commands[0]);

void cmd_info(void)
{
    // печатаем паспорт устройства
	device_info();
}

void cmd_version(void)
{
    // печатаем строку журнала о версии прошивки
	log_version();
}

void cmd_ping(void)
{
    // печатаем строку журнала о версии прошивки
	printf("pong\n");
}

void cmd_mem_info(void)
{
	mem_info();
}

void cmd_fw_info(void)
{
	fw_info();
}

void cmd_dev_info(void)
{
	dev_info();
}

void cmd_boot_info(void)
{
	boot_info();
}

void cmd_clk_info(void)
{
	clk_info();
}

void cmd_uptime(void)
{
	uptime();
}

void cmd_main_time_exec(void)
{
	printf("iteration avg %.2f us, max %u us\n", profiling_avg_us(), (unsigned)profiling_max_us());
}

void cmd_main_time_reset(void)
{
	profiling_reset_max();
	printf("max reset\n");
}
// -------------------

void handle_command(const char *command)
{
    for (uint i = 0; i < command_count; i++)
    {
        if (strcmp(command, commands[i].name) == 0)
        {
            if (commands[i].handler != NULL)
            {
                commands[i].handler();
            }

            return;
        }
    }

    LOG_ERR("unknown command: %s\n", command);
}

void read_line(void)
{
    int symbol = getchar_timeout_us(0);

    if (symbol == PICO_ERROR_TIMEOUT)
    {
        return;
    }

    if (symbol == '\r' || symbol == '\n')
    {
        putchar('\n');
        line[line_length] = '\0';

        if (line_length > 0)
        {
            LOG_DBG("got %s\n", line);
            handle_command(line);
        }

        line_length = 0;
        return;
    }
/*	
	if (symbol == '\b')
	{	
		line_length = line_length - 1;		//Укорачиваем набранную строчку на 1 символ
		if (line_length < 0)
			line_length = 0;
		putchar(symbol);	//Сотрет на экране
		return;
	}	
*/	
    if (line_length + 1 < LINE_SIZE)
    {
        line[line_length] = (char)symbol;
        line_length = line_length + 1;
        putchar(symbol);
    }
}

void blink(void)
{
    uint64_t now_us = time_us_64();

    if (now_us - last_toggle_us >= BLINK_HALF_PERIOD_MS * 1000)
    {
        last_toggle_us = now_us;
        led_toggle();
    }
}

double calc_pi(uint terms)
{
    // сумма ряда и знак очередного члена, оба double
	double summ = 0;
	double sign = 1;
    // для k от 0 до terms: прибавить к сумме sign / (2k + 1) и сменить знак
	for (int k = 0; k < terms; k++)
	{
		summ += sign / (2.0 * k + 1.0);
        sign = -sign;
	}
    // вернуть сумму, умноженную на 4
	return 4.0*summ;
}

void cmd_calc_pi(void)
{
    uint64_t start_us = time_us_64();
    pi_result = calc_pi(CALC_PI_TERMS);
    uint64_t spent_us = time_us_64() - start_us;

    printf("pi: %.8f\n", pi_result);
    printf("time: %llu ms\n", spent_us / 1000);
}

//-----------------

int main()
{
	stdio_init_all();
	led_init();
	profiling_init();
	
    while (1)
    {
        profiling_iteration();
		blink();
	    
		read_line();
  	
    }
	
	
}