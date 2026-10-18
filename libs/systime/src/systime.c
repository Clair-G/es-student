#include "pico/stdlib.h"

#include "systime/systime.h"

bool systime_perio_elapsed(uint64_t *last_us, uint64_t period_us)
{
    uint64_t now_us = time_us_64();

    if (now_us - *last_us <= period_us) 
	{
		return false;
	}

    // Следующее срабатывание отсчитывается от прошлого, а не от «сейчас»
    *last_us += period_us;

    // Отстали больше чем на период: не догонять, а начать отсчёт заново
    if (now_us - *last_us >= period_us) {
        *last_us = now_us;
    }

    return true;
}

uint64_t systime_us(void)
{
	return time_us_64();
}

uint64_t systime_ms(void)
{
	return time_us_64() / 1000;
}
