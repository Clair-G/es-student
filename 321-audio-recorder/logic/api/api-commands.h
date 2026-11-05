#pragma once

#include "stdio-text-protocol/stdio-text-protocol.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
//#include <errno.h>
#include <ctype.h>

typedef void (*api_callback_t)(const command_t *command);

typedef struct
{
    const char *name;
    const char *help;
    api_callback_t callback;
} api_command_t;

extern const api_command_t api_commands[];

//void api_handle(const command_t *command);

extern const uint32_t api_command_count;