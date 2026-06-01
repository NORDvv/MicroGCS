#include "command_parser.h"

static void CommandParser_ClearResponse(char* response_buffer, int response_buffer_size) {
    if (response_buffer != 0 && response_buffer_size > 0) {
        response_buffer[0] = '\0';
    }
}

static void CommandParser_WriteInvalidArgument(char* response_buffer, int response_buffer_size) {
    snprintf(response_buffer, (size_t)response_buffer_size, "ERR;cmd=UNKNOWN;reason=INVALID_ARGUMENT\r\r");
}

CommandResult CommandParser_Handle(
    const char* command,
    VehicleState* state,
    char* response_buffer,
    int response_buffer_size
) {
    CommandParser_ClearResponse(response_buffer, response_buffer_size);

    if (command == 0 || state == 0 || response_buffer == 0 || response_buffer_size <= 0) {
        CommandParser_WriteInvalidArgument(response_buffer, response_buffer_size);
        return CMD_RESULT_INVALID_ARGUMENT;
    }

    if (strcmp(command, "CMD;ARM") == 0) {
        state->armed = 1;

        snprintf(response_buffer, (size_t)response_buffer_size,"ACK;cmd=ARM;result=OK\r\n");

        return CMD_RESULT_OK;
    }

    if (strcmp(command,"CMD;DISARM") == 0) {
        state->armed = 0;

        snprintf(response_buffer, (size_t)response_buffer_size,"ACK;cmd=DISARM;result=OK\r\n");
        return CMD_RESULT_OK;
    }

    if (strcmp(command, "CMD;RESET") == 0) {
        VehicleState_Reset(state);

        snprintf(response_buffer,(size_t)response_buffer_size,"ACK;cmd=RESET;result=OK\r\n");
        return CMD_RESULT_OK;
    }

    if (strncmp(command,"CMD;MODE=",9) == 0) {
        const char* mode = command + 9;

        if (mode[0] == '\0') {
            snprintf(response_buffer,(size_t)response_buffer_size,"ERR;cmd=MODE;reason=INVALID_ARGUMENT\r\n");
            return CMD_RESULT_INVALID_ARGUMENT;
        }

        if (strcmp(mode, "MANUAL") == 0) {
            state->mode = MODE_MANUAL;

            snprintf(response_buffer,(size_t)response_buffer_size,"ACK;cmd=MODE;result=OK;mode=MANUAL\r\n");
            return CMD_RESULT_OK;
        }

        if (strcmp(mode, "AUTO") == 0) {
            state->mode = MODE_AUTO;

            snprintf(response_buffer,(size_t)response_buffer_size,"ACK;cmd=MODE;result=OK;mode=AUTO\r\n");
            return CMD_RESULT_OK;
        }

        if (strcmp(mode, "RTL") == 0) {
            state->mode = MODE_RTL;

            snprintf(response_buffer,(size_t)response_buffer_size,"ACK;cmd=MODE;result=OK;mode=RTL\r\n");
            return CMD_RESULT_OK;
        }

        snprintf(response_buffer,(size_t)response_buffer_size,"ERR;cmd=UNKNOWN;reason=INVALID_MODE\r\n");
        return CMD_RESULT_INVALID_MODE;
    }

    snprintf(response_buffer,(size_t)response_buffer_size,"ERR;cmd=UNKNOWN;reason=INVALID_COMMAND\r\n");
    return CMD_RESULT_INVALID_COMMAND;
}

/*
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
}*/