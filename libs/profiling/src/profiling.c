#include  "pico/stdlib.h"
#include "profiling/profiling.h"


typedef struct {
	uint32_t counts;
	uint32_t started;
	uint32_t summ;
	uint32_t max;
	const char *name;
} profiling_stopwatch_t;

static uint32_t stopwatches_num;

static profiling_stopwatch_t stopwatches[STOPWATCH_MAX_NUM];

//Переполнение через 71 минуту


void profiling_stopwatch_init(uint32_t id, const char *name)
{
	stopwatches[id].summ = 0;
	stopwatches[id].name = name;
	stopwatches[id].counts = 0;
	stopwatches[id].max = 0;
	stopwatches_num++;

}

#if PROFILING_ENABLED
void profiling_start(uint32_t id)
{
	stopwatches[id].started = time_us_32();

}

void profiling_stop(uint32_t id)
{
	uint32_t duration = time_us_32() - stopwatches[id].started;
	stopwatches[id].summ +=  duration;
	stopwatches[id].counts++;
	if ( duration > stopwatches[id].max)
		stopwatches[id].max = duration;

}
#endif

bool profiling_get(uint32_t id, profiling_result_t *result)
{
	if (id >= stopwatches_num)
		return false;
	result->name = stopwatches[id].name;
	result->max = stopwatches[id].max;
	result->counts = stopwatches[id].counts;
	result->mean = (float)stopwatches[id].summ / stopwatches[id].counts;
	result->name = stopwatches[id].name;
	return true;
}


void profiling_reset(void)
{
	for (int i = 0; i < STOPWATCH_MAX_NUM; i++)
	{
		stopwatches[i].counts = 0;
		stopwatches[i].summ = 0;
		stopwatches[i].max = 0;
	}
}