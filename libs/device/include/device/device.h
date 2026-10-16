#pragma once

#include <stdint.h>



#ifndef DEVICE_BOARD
#define DEVICE_BOARD "unknown"
#endif

//void device_info(void);


/*
struct info_t
{

    uint32_t version;
    char name[13];
    uint8_t revision;
	
};
*/

typedef struct 
{
	uint32_t manufacturer;
	uint32_t part;
    uint32_t revision;
	const char *board_type; //тип платы
	char board_id[17]; //серийный номер платы	
	const char *SDK_version;
} device_info_t;

void device_get_info(device_info_t *info);
