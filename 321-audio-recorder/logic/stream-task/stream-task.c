#include "stream-task.h"

#include "platform/platform.h"
#include "logging/log.h"

#include <stddef.h>

static stream_task_callback_t on_frame_cb;
static stream_state_t stream_state;
static uint8_t stream_bits;
static uint16_t stream_frame_num;
static uint32_t stream_count_sum;
//static stream_header_t header;


void stream_task_init(stream_task_callback_t on_frame)
{
	on_frame_cb = on_frame;
	stream_state = STREAM_STATE_IDLE;
		
}

bool stream_task_start(uint8_t bits)
{
	if ((bits != 8) && (bits != 16))
	{
        return false;
	}
	
	if (!stream_state == STREAM_STATE_IDLE || platform_stream_start(bits) == false)
		return false;
	
	
	stream_bits = bits;
	stream_frame_num = 0;
	stream_count_sum = 0;
	stream_state = STREAM_STATE_RUN;
	return true;
}

void stream_task_stop(void)
{
	platform_stream_stop();
	stream_state = STREAM_STATE_IDLE;
}

void stream_task_handle(void)
{
	switch (stream_state)
		{
			case STREAM_STATE_IDLE:
				break;
			case STREAM_STATE_RUN:
			{
				uint32_t samples;
				const uint8_t *data = (const uint8_t *)platform_stream_next(&samples);
				
				if (data) //!=NULL
				{
					stream_header_t header;
					header.sync_word = (uint16_t)STREAM_SYNC_WORD;
					header.number = stream_frame_num;
				//	header.count = (uint16_t)PLATFORM_STREAM_BYTES / (stream_bits/8);
					header.count = (uint16_t)samples;
					const stream_header_t *p_header = &header;
					
					uint32_t bytes = samples * (stream_bits / 8);
					on_frame_cb(p_header, data, bytes);
					platform_stream_release();
					stream_frame_num++;
					stream_count_sum += samples;
				}
				break;
			}
		}
		
}

stream_state_t stream_task_get_state(void)
{
	return stream_state;
}

uint8_t stream_task_get_bits(void)
{
	return stream_bits;
}

// итог потока с последнего запуска
uint32_t stream_task_get_frames(void)
{
	return stream_frame_num;
}

uint32_t stream_task_get_samples(void)
{
	return stream_count_sum;
}

uint32_t stream_task_get_overruns(void)
{
	return platform_stream_overruns();
}

