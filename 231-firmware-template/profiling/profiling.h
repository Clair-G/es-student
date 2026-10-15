#pragma once

#include "pico/types.h"

#define STOPWATCH_MAX_NUM 5 

void profiling_stopwatch_init(uint32_t id, const char *name);

void profiling_start(uint32_t id);
void profiling_stop(uint32_t id);

typedef struct {
	uint64_t counts;
	float mean;
	uint32_t max;
	const char *name;
} profiling_result_t;

// false — секундомер с таким номером не заведён
bool profiling_get(uint32_t id, profiling_result_t *result);
void profiling_reset(void);