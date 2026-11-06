#include "block-task.h"
#include "platform/platform.h"

//#include "systime/systime.h"
#include "logging/log.h"

static block_task_callback_t on_block_cb;
static uint32_t counts;
//static const uint16_t *samples;

// false — АЦП занят или блок больше, чем помещается в буфер
bool block_task_start(uint32_t count, block_task_callback_t on_block)
{
	if (platform_adc_busy())
		return false;
		
	if (!platform_block_start(count))
		return false;
	
	on_block_cb = on_block;
	counts = count;

	return true;
}

// вызывается на каждой итерации суперцикла, не ждёт
void block_task_handle(void)
{
	if (!platform_block_done() || on_block_cb == NULL)
	
		return;
	
	
	block_task_callback_t temp_func = on_block_cb;
	on_block_cb = NULL;
	
	temp_func(platform_block_samples(), counts, platform_block_time_us());
	
}

