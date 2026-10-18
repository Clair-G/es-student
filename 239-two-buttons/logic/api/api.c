#include "api.h"
#include "api-commands.h"

#include <stdlib.h>
#include <stdio.h>

#include <string.h>
#include "logging/log.h"

void api_handle(const command_t *command)
{	
	if (command->truncated)
		printf("error: command longer than 63 characters or 4 arguments\n");		
	else 
	if (strcmp(command->name, "help") == 0)
	{	
		printf("%20s %30s\n", "help", "list of commands");
		for (int i = 0; i < api_command_count; i++)
			printf("%20s %30s\n", api_commands[i].name, api_commands[i].help);
	}
	else //not help
	{
		for (uint32_t i = 0; i < api_command_count; i++)
		{
			if (strcmp(command->name, api_commands[i].name) == 0)
			{
				api_commands[i].callback(command);
				return;
			}
		}
		printf("error: unknown command '%s', try help\n", command->name);
	}
}
