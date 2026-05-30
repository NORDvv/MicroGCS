#include "command_parser.h"

CommandResult CommandParser_Handle(
    const char* command,
    VehicleState* state,
    char* response_buffer,
    int response_buffer_size
) {}

uint8_t CommandParser_ApplyLine(const char* line, VehicleState* state)
{
    if (line == 0 || state == 0)
    {
        return 0;
    }

    if (strcmp(line, "CMD;ARM") == 0)
    {
        state->armed = 1;
        return 1;
    }

    if (strcmp(line, "CMD;DISARM") == 0)
    {
        state->armed = 0;
        return 1;
    }

    if (strcmp(line, "CMD;MODE=MANUAL") == 0)
    {
        state->mode = MODE_MANUAL;
        return 1;
    }

    if (strcmp(line, "CMD;MODE=AUTO") == 0)
    {
        state->mode = MODE_AUTO;
        return 1;
    }

    if (strcmp(line, "CMD;MODE=RTL") == 0)
    {
        state->mode = MODE_RTL;
        return 1;
    }

    if (strcmp(line, "CMD;RESET") == 0)
    {
        VehicleState_Reset(state);
        return 1;
    }

    return 0;
}