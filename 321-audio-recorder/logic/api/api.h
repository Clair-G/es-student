#pragma once

#include "stdio-text-protocol/stdio-text-protocol.h"
#include "telemetry-task/telemetry-task.h"

//typedef void (*api_callback_t)(const command_t *command);

void api_handle(const command_t *command);
/*
typedef struct
{
    const char *name;
    const char *help;
    api_callback_t callback;
} api_command_t;
*/
//extern const api_command_t api_commands[];
//extern const uint api_command_count;

// печатает запись телеметрии строкой tm: <время, мкс> <напряжение, В> <температура, °C>
void api_print_telemetry(const telemetry_record_t *record);