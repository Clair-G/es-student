#include "api-commands.h"

#include "firmware.h"

#include "led-task/led-task.h"
#include "button-task/button-task.h"
#include "pi-task/pi-task.h"
#include "platform/platform.h" 
#include "telemetry-task/telemetry-task.h"

#include "profiling/profiling.h"
#include "systime/systime.h"
#include "device/device.h"
#include "logging/log.h"

#include <string.h>
#include <errno.h>
#include <ctype.h>

#define TELEMETRY_DEFAULT_PERIOD_MS 100

static void cmd_info(const command_t *command);
static void cmd_uptime(const command_t *command);
static void cmd_profiling(const command_t *command);
static void cmd_profiling_reset(const command_t *command);
static void cmd_led_enable(const command_t *command);
static void cmd_led_disable(const command_t *command);
static void cmd_led_blink(const command_t *command);
static void cmd_led_period(const command_t *command);
static void cmd_button(const command_t *command);
static void cmd_pi_start(const command_t *command);
static void cmd_pi(const command_t *command);
static void cmd_get_voltage(const command_t *command);
static void cmd_get_temp(const command_t *command);
static void cmd_tm_start(const command_t *command);
static void cmd_tm_stop(const command_t *command);
static void cmd_charge_on(const command_t *command);
static void cmd_charge_off(const command_t *command);

const api_command_t api_commands[] = {
    { "info", "device passport", cmd_info },
	{ "uptime", "time since reset", cmd_uptime },
	{ "led_enable", "led_enable", cmd_led_enable },
	{ "led_disable", "led_disable", cmd_led_disable },
	{ "led_blink", "led_blink", cmd_led_blink },
	{ "led_period", "led_period <period_ms>", cmd_led_period },
	{ "button", "button", cmd_button },
	{ "pi_start", "calculate pi: pi_start [<terms>]", cmd_pi_start },
	{ "pi", "returns pi: pi", cmd_pi },
	{ "profiling", "stopwatches", cmd_profiling },
	{ "profiling_reset", "reset stopwatches", cmd_profiling_reset },
	{ "get_voltage", "voltage on GP26, ADC input 0", cmd_get_voltage },
	{ "get_temp", "chip temperature, degrees Celsius", cmd_get_temp },
	{ "tm_start", "tm_start [<period_ms>]", cmd_tm_start},
	{ "tm_stop", "tm_stop", cmd_tm_stop },
	{ "charge_on", "charge on", cmd_charge_on },
	{ "charge_off", "charge off", cmd_charge_off },
};

const uint32_t api_command_count = sizeof(api_commands) / sizeof(api_commands[0]);

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


static void print_led_state(void)
{
	led_state_t led_state = led_task_get_state();
	switch (led_state)
	{
		case LED_STATE_OFF:
		{
			printf("led: off, period %lu ms\n", led_task_get_period_ms());
			break;
		}
		case LED_STATE_ON:
		{
			printf("led: on, period %lu ms\n", led_task_get_period_ms());
			break;
		}
		case LED_STATE_BLINK:
		{
			printf("led: blink, period %lu ms\n", led_task_get_period_ms());
			break;
		}
	}
	
	return;
}

void api_print_telemetry(const telemetry_record_t *record)
{
	printf("tm: %llu %.3f %.1f\n", record->time_us, record->voltage_v, record->temp_c);
}

static void cmd_info(const command_t *command)
{
    device_info_t info;
	device_get_info(&info);
	// печатаем паспорт устройства
	printf("name: %s\n", FIRMWARE_NAME);
	printf("version: %s\n", FIRMWARE_VERSION);
	// печатаем пять строк паспорта
	printf("project: %s\n", FIRMWARE_PROJECT);
    printf("repo: %s\n", FIRMWARE_REPO);
    printf("board: %s\n", info.board_type);//!
    printf("serial: %s\n", info.board_id);
    printf("chip: manufacturer 0x%03lx, part 0x%04lx, revision %lu\n", info.manufacturer, info.part, info.revision);
	printf("pico-sdk: %s\n", info.SDK_version);
}

static void cmd_uptime(const command_t *command)
{
	uint64_t uptime = systime_ms();
	printf("uptime: %llu ms\n", uptime);
}

static void cmd_pi_start(const command_t *command)
{
	uint32_t terms;
	if (command->argc == 0)
	{
		terms = pi_task_get_def_terms_num();
	}
	else
	{		
		if (command->argc > 1 || !parse_u32(command->argv[0], &terms))
		{
			printf("error: usage pi_start [terms]\n");
			return;
		}
		if (!terms)
		{
			printf("error: terms must be greater than 0\n");
			return;
		}
	}	 
	
	if (pi_task_start(terms))
		printf("pi: started, %ld terms\n", terms);
	else 
		printf("error: pi is busy\n");
   
}

static void cmd_pi(const command_t *command)
{	
	double result;
	uint32_t duration;
	if (pi_task_get_result(&result, &duration))
	{	
		printf("pi: %.9f in %lu ms\n", result, duration);
		return;
	}
	else if (!duration)
    {
        printf("pi: not started\n");
        return;
    }
	else 
		printf("pi: running\n");
   
}

static void cmd_profiling(const command_t *command)
{
	#if PROFILING_ENABLED
	uint32_t id = 0;
	profiling_result_t result;
	while (profiling_get(id, &result))
	{	
		printf("%s count %llu mean %.2f us max %lu us\n", 
				result.name, result.counts, result.mean, result.max);
		id++;
	}
	#else
	// профилирование выключено: вызовы ничего не делают
	printf("profiling is disabled in this build\n");
	#endif
}

static void cmd_profiling_reset(const command_t *command)
{
	profiling_reset();
	printf("profiling reset\n");
}

static void cmd_led_enable(const command_t *command)
{
	led_task_set_state(LED_STATE_ON);
	print_led_state();
}

static void cmd_led_disable(const command_t *command)
{
	led_task_set_state(LED_STATE_OFF);
	print_led_state();
}

static void cmd_led_blink(const command_t *command)
{
	led_task_set_state(LED_STATE_BLINK);
	print_led_state();
}

static void cmd_led_period(const command_t *command)
{
    uint32_t period_ms;

    if (command->argc != 1 || !parse_u32(command->argv[0], &period_ms))
    {
        printf("error: usage led_period <period_ms>\n");
        return;
    }

    // годится ли такой период, решает задача светодиода
    if (!led_task_set_period_ms(period_ms))
    {
        printf("error: period_ms must be greater than 0\n");
        return;
    }

    // ответ: состояние и период светодиода
	print_led_state();
}

static void cmd_button(const command_t *command)
{
	uint32_t id = 0;
	
	button_info_t info;
	while (button_task_get_info(id, &info))
	{	
		char *state; //max string length
		if (info.state) state = "pressed";
		else state = "released";
		
		printf("button %lu: %s, presses %llu\n", 
				(id + 1), state, info.presses);
		id++;
	}

	return;
}

static void cmd_get_voltage(const command_t *command)
{
	float voltage = platform_voltage_read();
	printf("voltage: %.03f V\n", voltage);
	
	return;
}

static void cmd_get_temp(const command_t *command)
{
	float temp = platform_temp_read();
	printf("temp: %.01f C\n", temp);
	
	return;
}

static void cmd_tm_start(const command_t *command)
{
	uint32_t period_ms;

	if (command->argc == 0)
	{
		period_ms = (uint32_t)TELEMETRY_DEFAULT_PERIOD_MS;
	}	
	else
	{	
		if (command->argc > 1 || !parse_u32(command->argv[0], &period_ms))
		{
			printf("error: usage tm_start [<period_ms>]\n");
			return;
		}
	}	
	
	if (telemetry_task_start(period_ms))	
		printf("telemetry: started, period %lu ms\n", period_ms);
	else 
		printf("error: period_ms must be at least 10\n");

}

static void cmd_tm_stop(const command_t *command)
{
	telemetry_task_stop();
	printf("telemetry: stopped\n");
}

static void cmd_charge_on(const command_t *command)
{
	platform_charge_set(true);
	printf("charge: on\n");
}
static void cmd_charge_off(const command_t *command)
{
	platform_charge_set(false);
	printf("charge: off\n");
}