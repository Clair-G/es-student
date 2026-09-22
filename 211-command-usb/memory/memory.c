#include <stdio.h>
#include <stdint.h>
#include "hardware/regs/addressmap.h"
#include "pico/stdlib.h"
#include "memory.h"

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
}