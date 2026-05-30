#ifndef COMMAND_PARSER_H
#define COMMAND_PARSER_H

#include "vehicle_state.h"
#include <stdint.h>
#include <string.h>

typedef enum {
    CMD_RESULT_OK,
    CMD_RESULT_INVALID_COMMAND,
    CMD_RESULT_INVALID_MODE,
    CMD_RESULT_INVALID_ARGUMENT
} CommandResult;

CommandResult CommandParser_Handle(
    const char* command,
    VehicleState* state,
    char* response_buffer,
    int response_buffer_size
);

uint8_t CommandParser_ApplyLine(const char* line, VehicleState* state);

#endif