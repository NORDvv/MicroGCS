#ifndef TELEMETRY_GENERATOR_H
#define TELEMETRY_GENERATOR_H

#include "vehicle_state.h"
#include <stdio.h>

void TelemetryGenerator_BuildPacket(
    const VehicleState* state,
    char* buffer,
    int buffer_size
);

#endif