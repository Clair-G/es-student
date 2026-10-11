#include "api.h"
#include "led.h"
#include "device.h"
#include "profiling.h"
#include "pico/stdlib.h"
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <stdlib.h>

static void cmd_info(const command_t *command);
static void cmd_uptime(const command_t *command);
static void cmd_calc_pi(const command_t *command);
static void cmd_main_time_exec(const command_t *command);
static void cmd_main_time_reset(const command_t *command);

static bool parse_u32(const char *text, uint32_t *value);
double calc_pi(uint terms);

const uint CALC_PI_TERMS = 1000000;

volatile double pi_result;

const api_command_t api_commands[] = {
    { "info", "device passport", cmd_info },
	{ "uptime", "time since reset", cmd_uptime },
	{ "calc_pi", "calculate pi: calc_pi [terms]", cmd_calc_pi },
	{ "main_time_exec", "superloop iteration: average and maximum", cmd_main_time_exec },
	{ "main_time_reset", "reset maximum iteration", cmd_main_time_reset },
};

const uint api_command_count = sizeof(api_commands) / sizeof(api_commands[0]);


void cmd_info(const command_t *command)
{
    // печатаем паспорт устройства
	device_info();
}

void cmd_uptime(const command_t *command)
{
	uint64_t uptime = time_us_64() / 1000;
	printf("uptime: %llu ms\n", uptime);
}	
	
void cmd_calc_pi(const command_t *command)
{
    uint64_t start_us = time_us_64();
	uint32_t terms;
    if (command->argc > 1 || !parse_u32(command->argv[0], &terms))
    {
        printf("error: usage calc_pi [terms]\n");
        return;
    }
	if (!terms)
    {
        printf("error: terms must be greater than 0\n");
        return;
    }
		
	pi_result = calc_pi(CALC_PI_TERMS);
    uint64_t spent_us = time_us_64() - start_us;
	
	
    printf("pi: %.8f\n", pi_result);
    printf("time: %llu ms\n", spent_us / 1000);
}

void cmd_main_time_exec(const command_t *command)
{
	printf("iteration avg %.2f us, max %u us\n", profiling_avg_us(), (unsigned)profiling_max_us());
}

void cmd_main_time_reset(const command_t *command)
{
	profiling_reset_max();
	printf("max reset\n");
}

void api_handle(const command_t *command)
{	
	if (command->truncated)
		LOG_ERR("error: command longer than 63 characters or 4 arguments\n");		
	else 
	if (strcmp(command->name, "help") == 0)
	{	
		printf("%20s %30s\n", "help", "list of commands");
		for (int i = 0; i < api_command_count; i++)
			printf("%20s %30s\n", api_commands[i].name, api_commands[i].help);
	}
	else //not help
	{
		for (uint32_t i = 0; i < api_command_count; i++)
		{
			if (strcmp(command->name, api_commands[i].name) == 0)
			{
				api_commands[i].callback(command);
				return;
			}
		}
		printf("error: unknown command '%s', try help\n", command->name);
	}
}

// Число без знака из десятичной записи. false — в записи не только цифры
// или число не помещается в uint32_t.
static bool parse_u32(const char *text, uint32_t *value)
{
    // strtoul приняла бы и знак: из "-1" получилось бы 4294967295
    if (!isdigit((unsigned char)text[0]))
    {
        return false;
    }

    char *end;
    errno = 0;
    unsigned long number = strtoul(text, &end, 10);

    // после числа остались символы или число не поместилось
    if (*end != '\0' || errno == ERANGE || number > UINT32_MAX)
    {
        return false;
    }

    *value = (uint32_t)number;
    return true;
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
