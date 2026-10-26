#include "pi-task/pi-task.h"

#include "systime/systime.h"

#define PI_TERMS_PER_CALL 100
#define PI_TERMS_DEFAULT_NUM 1000000


// Состояние расчёта: должно пережить возврат из обработчика

static pi_state_t pi_state;

static uint32_t pi_k;
static uint32_t pi_n;
static double pi_sum;
static double pi_sign;
static double pi_result;
static uint64_t start_time;
static uint32_t pi_duration;

void pi_task_init(void)
{
	pi_state = PI_STATE_OFF;
	
}

bool pi_task_start(uint32_t n)
{
    switch (pi_state)
	{
		case PI_STATE_RUNNING:
		{
			return false;
		}
		case PI_STATE_READY:
		case PI_STATE_OFF:
		{
			if (n == 0) 
				n = PI_TERMS_DEFAULT_NUM;
			pi_n = n;
			pi_k = 0;
			pi_sum = 0.0;
			pi_sign = 1.0;
			pi_duration = 1; //признак "работает"
			pi_state = PI_STATE_RUNNING;
			start_time = systime_us();
			return true;
		}
	}
	return false;
}	

void pi_task_handle(void)
{
    switch (pi_state)
	{
		case PI_STATE_OFF:
		case PI_STATE_READY:
		{
			return;
		}
		case PI_STATE_RUNNING:
		{
			// Очередная порция членов ряда, но не дальше последнего
			int end = pi_k + PI_TERMS_PER_CALL;
			if (end > pi_n) 
			{
				end = pi_n;
			}
			for (; pi_k < end; pi_k++) 
			{
				pi_sum += pi_sign / (2.0 * pi_k + 1.0);
				pi_sign = -pi_sign;
			}
			if (pi_k == pi_n) 
			{
				pi_result = 4.0 * pi_sum;
				pi_duration =  (uint32_t)((systime_us() - start_time) / 1000);
				pi_state = PI_STATE_READY;
				
			}
		}
	}
}

// Готов ли результат: если да, записать его по указателю
bool pi_task_get_result(double *result, uint32_t *duration)
{
    switch (pi_state)
	{
		case PI_STATE_READY:
		{
			*result = pi_result;
			*duration = pi_duration;
			return true;
		}
		case PI_STATE_OFF:
		{
			*result = 0;
			*duration = 0;
		}
		case PI_STATE_RUNNING:
		{
			*result = pi_result;
			*duration = pi_duration;
		}
	}
	return false;
}

uint32_t pi_task_get_def_terms_num()
{
	return PI_TERMS_DEFAULT_NUM;
}