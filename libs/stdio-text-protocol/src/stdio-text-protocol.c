#include "stdio-text-protocol/stdio-text-protocol.h"
#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "logging/log.h"


#define LINE_SIZE 32

static command_t command;

char line [STDIO_TEXT_PROTOCOL_LINE_SIZE];
uint line_length;

void parse_command(void);

void stdio_text_protocol_init(void)
{
	line_length = 0; 
	command.truncated = false;
}

const command_t *stdio_text_protocol_handle(void)
{
	// читаем символ без ожидания; нет символа — возвращаем NULL
	int symbol = getchar_timeout_us(0);

    if (symbol == PICO_ERROR_TIMEOUT)
    {
        return NULL;
    }

    // конец строки: пустую строку пропускаем, иначе переводим строку в терминале,
    // делим строку на слова и возвращаем команду
	if (symbol == '\r' || symbol == '\n')
    {
        if (line_length == 0)
			return NULL;
		
		putchar('\n');
        line[line_length] = '\0';

    //    LOG_DBG("got %s\n", line);
		
        if (command.truncated == false)
			parse_command();
		line_length = 0;
		return &command;
    }
    
	// Backspace (0x08 или 0x7f): убираем последний символ и стираем его на экране "\b \b"
	if (symbol == 0x08 || symbol == 0x7f)
	{
		if (line_length)
			line_length = line_length - 1;
		printf("\b \b");
		
	}
    
	// печатный символ: кладём в буфер и повторяем в терминал;
	if (symbol >= 0x20 && symbol <=0x7e)
	{
		if (line_length + 1 < STDIO_TEXT_PROTOCOL_LINE_SIZE)
		{
			command.truncated = false;
			line[line_length] = (char)symbol;
			line_length = line_length + 1;
			putchar(symbol);
		}
		// буфер полон — помечаем строку как переполненную
		//if (line_length >= STDIO_TEXT_PROTOCOL_LINE_SIZE)
		else
		{
			command.truncated = true;
		}
	}
	
	return NULL;
}

void parse_command(void)
{	
	command.name = '\0';
	command.argc = 0;
	
	for (int i = 0; i < line_length; i++)
		{
			if (line[i] == ' ') //change separator
			{	
				line[i] = '\0';
			} 
			else
			{
				if (line[i] != '\0') //normal symbol
				{
					if (i) //not the first
					{
						if (line[i-1] == '\0')
						{
							command.argv[command.argc]=&line[i];
							command.argc++;
						}
					}						
					else // FIRST
					{
						command.name=&line[i];
						command.argc = 0;
					}
				}
			}
			
			if (command.argc > 5) 
			{ 	
				command.truncated = true;
				command.argc--;
				break;
			}
		}
/*	
	LOG_DBG("out of the cycle, command.argc= %u\n", command.argc);
	for (int i = 0; i < command.argc; i++)
	LOG_DBG("%u, %s\n", i, command.argv[i]);
*/
	return;
}
