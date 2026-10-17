#pragma once

#include <stdbool.h>
#include <stdint.h>


typedef enum {
    PI_STATE_OFF,
    PI_STATE_RUNNING,
    PI_STATE_READY,
} pi_state_t;

void pi_task_init(void);
void pi_task_handle(void);

// If n == 0, default PI_TERMS_DEFAULT_NUM 1000000 is used
bool pi_task_start(uint32_t n);
bool pi_task_get_result(double *result, uint32_t *duration);
uint32_t pi_task_get_def_terms_num();