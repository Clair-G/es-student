#include "telemetry-task.h"
#include "logging/log.h"
#include "platform/platform.h"
#include "systime/systime.h"

static const uint32_t DEFAULT_TELEMETRY_PERIOD_MS=100;

static telemetry_task_callback_t telemetry_task_record_cb;
static telemetry_state_t telemetry_state;

static uint64_t telemetry_period_us;
static uint64_t last_us;

static telemetry_record_t record;


void telemetry_task_init(telemetry_task_callback_t on_record)
{
	if (!on_record)
	{
		LOG_ERR("telemetry_task_callback NULL pointer\n");
		return;
	}
	telemetry_task_record_cb = on_record;
	telemetry_period_us = DEFAULT_TELEMETRY_PERIOD_MS*1000;
	telemetry_state = TELEMETRY_STATE_IDLE;
	
	return;
}

bool telemetry_task_start(uint32_t period_ms)
{	
	if (period_ms < 10)
		return false;
	
	telemetry_period_us = period_ms*1000;
	last_us = 0;//??
	telemetry_state = TELEMETRY_STATE_RUN;
	return true;
}	

void telemetry_task_stop()
{
	telemetry_state = TELEMETRY_STATE_IDLE;
}


void telemetry_task_handle(void)
{
	switch (telemetry_state)
    {	
		case TELEMETRY_STATE_IDLE:
			break;
		case TELEMETRY_STATE_RUN:
			if (systime_period_elapsed(&last_us, telemetry_period_us)) 
			{
				record.time_us = systime_us();
				record.voltage_v = platform_voltage_read();
				record.temp_c = platform_temp_read();
				last_us = record.time_us;
				telemetry_task_record_cb(&record);
			}
			break;
    }
}

uint32_t telemetry_task_get_period_ms(void)
{
	return (uint32_t)telemetry_period_us / 1000;
}