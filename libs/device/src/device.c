#include "device/device.h"

#include <stdio.h>
#include <stddef.h>

#include "pico/unique_id.h"
#include "pico/version.h"
#include "hardware/regs/addressmap.h"
#include "hardware/regs/sysinfo.h"
/*
void device_info(void)
{
    // читаем серийный номер платы функцией SDK
	char board_id[PICO_UNIQUE_BOARD_ID_SIZE_BYTES * 2 + 1];
    pico_get_unique_board_id_string(board_id, sizeof(board_id));
	
	// читаем регистр CHIP_ID по адресу и разбираем три поля
	volatile uint32_t *chip_id = (uint32_t *)(SYSINFO_BASE + SYSINFO_CHIP_ID_OFFSET);
    uint32_t id = *chip_id;
	
	uint32_t manufacturer = (id & SYSINFO_CHIP_ID_MANUFACTURER_BITS) >> 
								SYSINFO_CHIP_ID_MANUFACTURER_LSB;
	
	uint32_t part = (id & SYSINFO_CHIP_ID_PART_BITS) >> 
								SYSINFO_CHIP_ID_PART_LSB;
	
	uint32_t revision = (id & SYSINFO_CHIP_ID_REVISION_BITS>> 
								SYSINFO_CHIP_ID_REVISION_LSB);	
								
	printf("name: %s\n", DEVICE_NAME);
	printf("version: %s\n", FIRMWARE_VERSION);
	// печатаем пять строк паспорта
	printf("project: %s\n", DEVICE_PROJECT);
    printf("repo: %s\n", DEVICE_REPO);
    printf("board: %s\n", DEVICE_BOARD);
    printf("serial: %s\n", board_id);
    printf("chip: manufacturer 0x%03lx, part 0x%04lx, revision %lu\n", manufacturer, part, revision);
	printf("pico-sdk: %s\n", PICO_SDK_VERSION_STRING);
}
*/
void device_get_info(device_info_t *info)
{
	// читаем серийный номер платы функцией SDK
	char board_id[PICO_UNIQUE_BOARD_ID_SIZE_BYTES * 2 + 1];
    pico_get_unique_board_id_string(board_id, sizeof(board_id));
	
	info->board_type = DEVICE_BOARD;
	for (int i = 0; i < PICO_UNIQUE_BOARD_ID_SIZE_BYTES * 2 + 1; i++)
		info->board_id[i] = board_id[i];

	
	// читаем регистр CHIP_ID по адресу и разбираем три поля
	volatile uint32_t *chip_id = (uint32_t *)(SYSINFO_BASE + SYSINFO_CHIP_ID_OFFSET);
		
    uint32_t id = *chip_id;
	
	info->manufacturer = (id & SYSINFO_CHIP_ID_MANUFACTURER_BITS) >> 
								SYSINFO_CHIP_ID_MANUFACTURER_LSB;
	
	info->part = (id & SYSINFO_CHIP_ID_PART_BITS) >> 
								SYSINFO_CHIP_ID_PART_LSB;
	
	info->revision = (id & SYSINFO_CHIP_ID_REVISION_BITS>> 
								SYSINFO_CHIP_ID_REVISION_LSB);	
	info->SDK_version = PICO_SDK_VERSION_STRING;						
}