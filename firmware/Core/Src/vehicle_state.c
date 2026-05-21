#include "vehicle_state.h"

void VehicleState_Init(VehicleState* state) {
    state->armed = 0;
    state->mode = MODE_MANUAL;
    state->battery = 100.0;

}

void VehicleState_Update(VehicleState* state, float dt_seconds) {}

void VehicleState_Reset(VehicleState* state) {}