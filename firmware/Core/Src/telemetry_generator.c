#include "telemetry_generator.h"
#include <stdio.h>


static const char* TelemetryGenerator_ModeToString(VehicleMode mode) {
    switch (mode) {
        case MODE_MANUAL:
            return "MANNUAL";

        case MODE_AUTO:
            return "AUTO";

        case MODE_RTL:
            return "RTL";

        default:
            return "UNKNOWN";
    }
}

static int TelemetryGenerator_ToTenths(float value) {
    if (value >= 0.0f) {
        return (int)(value * 10.0f + 0.5f);
    }

    return (int)(value * 10.0f - 0.5f);
}

// because %f was too much trouble
static void TelemetryGenerator_FormatFixed1(char* buffer, int buffer_size, float value) {
    if (buffer == 0 || buffer_size <= 0) {
        return;
    }

    int scaled = TelemetryGenerator_ToTenths(value);

    if (scaled < 0) {
        scaled = -scaled;
        snprintf(buffer, (size_t)buffer_size, "-%d.%d", scaled / 10, scaled % 10);
    } else {
        snprintf(buffer, (size_t)buffer_size, "%d.%d", scaled / 10, scaled % 10);
    }
}

void TelemetryGenerator_BuildPacket(
    const VehicleState* state,
    char* buffer,
    int buffer_size
) {
    if (buffer == 0 || buffer_size <= 0) {
        return;
    }

    if (state == 0) {
        snprintf(buffer, (size_t)buffer_size, "ERR;reason=NULL_STATE\r\n");
        return;
    }

    char battery_text[16];
    char altitude_text[16];
    char x_text[16];
    char y_text[16];

    TelemetryGenerator_FormatFixed1(battery_text, sizeof(battery_text), state->battery);
    TelemetryGenerator_FormatFixed1(altitude_text, sizeof(altitude_text), state->altitude);
    TelemetryGenerator_FormatFixed1(x_text, sizeof(x_text), state->x);
    TelemetryGenerator_FormatFixed1(y_text, sizeof(y_text), state->y);

    snprintf(
        buffer,
        (size_t)buffer_size,
        "TEL;armed=%d;mode=%s;batt=%s;alt=%s;x=%s;y=%s;wp=%d;status=%s\r\n",
        state->armed,
        TelemetryGenerator_ModeToString(state->mode),
        battery_text,
        altitude_text,
        x_text,
        y_text,
        state->waypoint,
        state->status
    );
}
