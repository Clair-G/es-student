#pragma once

uint64_t systime_us(void);
uint64_t systime_ms(void);
bool systime_perio_elapsed(uint64_t *last_us, uint64_t period_us);