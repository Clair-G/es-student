#pragma once

#include <stdbool.h>

// по умолчанию профилирование включено
#ifndef PROFILING_ENABLED
#define PROFILING_ENABLED 1
#endif

#if PROFILING_ENABLED
void profiling_start(uint32_t id);
void profiling_stop(uint32_t id);
#else
// профилирование выключено: вызовы ничего не делают
#define profiling_start(id) ((void)0)
#define profiling_stop(id) ((void)0)
#endif

#define STOPWATCH_MAX_NUM 5 

void profiling_stopwatch_init(uint32_t id, const char *name);

typedef struct {
	uint64_t counts;
	float mean;
	uint32_t max;
	const char *name;
} profiling_result_t;

// false — секундомер с таким номером не заведён
bool profiling_get(uint32_t id, profiling_result_t *result);
void profiling_reset(void);