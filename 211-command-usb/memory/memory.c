#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "hardware/regs/addressmap.h"
#include "pico/stdlib.h"

#include "memory.h"
#include "command.h"
#include "device.h"

extern uint32_t data_variable;
extern uint32_t bss_variable;

int main(void);

extern char __flash_binary_start;
extern char __flash_binary_end;
extern char __boot2_start__;
extern char __boot2_end__;
extern char __etext;
extern char __data_start__;
extern char __data_end__;
extern char __bss_start__;
extern char __bss_end__;
extern char __HeapLimit;
extern char __StackBottom;
extern char __StackTop;

static void row(const char *name, uintptr_t start, uintptr_t end)
{
    printf("%-10s 0x%08x 0x%08x %8u\n",
           name, (unsigned)start, (unsigned)end, (unsigned)(end - start));
}

void mem_info(void)
{
    // шапка таблицы: область, начало, конец, размер
	printf("%-10s %-10s %-10s %-10s\n",
           "area", "start", "end", "size");
   
    // flash — XIP_BASE и PICO_FLASH_SIZE_BYTES
	row("flash", XIP_BASE, (XIP_BASE + PICO_FLASH_SIZE_BYTES));
    
	// sram — базовый адрес из SDK, размер из datasheet
	row("sram", SRAM_BASE, (270336 + SRAM_BASE));
    
	// rom — базовый адрес из SDK, размер из datasheet
	row("rom", ROM_BASE, (16384 - ROM_BASE));
	
    // image — от __flash_binary_start до __flash_binary_end
	row("image", (uintptr_t)&__flash_binary_start, (uintptr_t)&__flash_binary_end);
    
	// free  — от __flash_binary_end до конца флеш-памяти
	row("free", (uintptr_t)&__flash_binary_end, (XIP_BASE + PICO_FLASH_SIZE_BYTES));
	
    // boot2 — от __boot2_start__ до __boot2_end__
	row("boot2", (uintptr_t)&__boot2_start__, (uintptr_t)&__boot2_end__);
	
    // text  — от __boot2_end__ до __etext: код и константы
	row("text", (uintptr_t)&__boot2_end__, (uintptr_t)&__etext);
	
    // data flash — хранение .data, от __etext, длиной с .data
	row("data flash", (uintptr_t)&__etext, ((uintptr_t)&__etext +
			(uintptr_t)&__data_end__ - (uintptr_t)&__data_start__));
	
	// data ram   — работа .data, от __data_start__ до __data_end__
	row("data ram", (uintptr_t)&__data_start__, (uintptr_t)&__data_end__);
	
    // bss        — от __bss_start__ до __bss_end__
	row("bss", (uintptr_t)&__bss_start__, (uintptr_t)&__bss_end__);
	
    // heap       — от __bss_end__ до __HeapLimit
	row("heap", (uintptr_t)&__bss_end__, (uintptr_t)&__HeapLimit);
	
    // stack      — от __StackBottom до __StackTop
	row("stack", (uintptr_t)&__StackBottom, (uintptr_t)&__StackTop);

	printf("\ntotal\n");
    // итог: образ во флеш и из чего он сложился
	printf("\tflash image\t %8u = boot2 %8u + text  %8u + data %8u\n",
			((&__boot2_end__ - &__boot2_start__) + (&__etext - &__boot2_end__) + 			(&__data_end__ - &__data_start__)),
				(&__boot2_end__ - &__boot2_start__),
			(&__etext - &__boot2_end__),
			(&__data_end__ - &__data_start__));
    // итог: свободно во флеш-памяти из всего её объёма
	printf("\tflash free\t %8u of %8u\n",
			(XIP_BASE + PICO_FLASH_SIZE_BYTES - (uintptr_t)&__flash_binary_end),
			(PICO_FLASH_SIZE_BYTES));
    // итог: занято в ОЗУ — .data и .bss
	printf("\tram used\t %8u = data %8u + bss %8u\n",
			((&__data_end__ - &__data_start__) + (&__bss_end__ - &__bss_start__)),
			(&__data_end__ - &__data_start__),
			(&__bss_end__ - &__bss_start__));
    // итог: свободно в ОЗУ — под кучу и под стек
	printf("\tram free\t %8u for heap and %8u for stack\n",
			(&__HeapLimit - &__bss_end__),
			(&__StackTop - &__StackBottom));
			
	return;
}

void fw_info(void)
{
	data_variable++;
	bss_variable++;
	
	printf("%-16s %-10s %-10s\n",
           "object", "address", "value");
		   
	uint16_t *main_code = (uint16_t *)((uintptr_t)main & ~1u);	   
	printf("%-16s 0x%08x 0x%08x\n", "main", main, *main_code);	
	
	uint16_t *fw_code = (uint16_t *)((uintptr_t)fw_info & ~1u);	   
	printf("%-16s 0x%08x 0x%08x\n", "fw_info", fw_info, *fw_code);	
	
	printf("%-16s 0x%08x\n", "commands", &commands);	
	for (uint i = 0; i < command_count; i++)
	{
	//	uint16_t *command_code_real = (uint16_t *)(uintptr_t)commands[i].handler;
		printf("- %-16s 0x%08x\n", commands[i].name, commands[i].handler);
	}
	

	printf("%-16s 0x%08x %-10s\n", "DEVICE_PROJECT", &DEVICE_PROJECT, DEVICE_PROJECT);
	printf("%-16s 0x%08x %-10s\n", "DEVICE_BOARD", &DEVICE_BOARD, DEVICE_BOARD);
	printf("%-16s 0x%08x %-10u\n", "data_variable", &data_variable, data_variable);
	printf("%-16s 0x%08x %-10u\n", "bss_variable", &bss_variable, bss_variable);
	
	uint32_t stack_variable = 1946;
	uint32_t *st_var = &stack_variable;
	printf("%-16s 0x%08x %-10u\n", "stack_variable", st_var, stack_variable);
	
    uint32_t *heap_variable = malloc(sizeof(uint32_t));

    if (heap_variable != NULL)
    {
        *heap_variable = 1951;
		printf("%-16s 0x%08x %-10u\n", "heap_variable", heap_variable, *heap_variable);
    }
	
	free(heap_variable);
	return;
}
