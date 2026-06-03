#include "vehicle_state.h"

static void VehicleState_SetStatus(VehicleState* state, const char* status) {
    if (state == 0 || status == 0) {
        return;
    }

    strncpy(state->status, status, sizeof(state->status) - 1);
    state->status[sizeof(state->status)-1] = '\0';
}

void VehicleState_Init(VehicleState* state) {
    if (state == 0) {
        return;
    }

    state->armed = 0;
    state->mode = MODE_MANUAL;
    state->battery = 100.00f;
    state->altitude = 0.0f;
    state->x = 0.0f;
    state->y = 0.0f;
    state->waypoint = 0;

    VehicleState_SetStatus(state, "OK");

}

void VehicleState_Update(VehicleState* state, float dt_seconds) {
    if (state == 0) {
        return;
    }

    if (dt_seconds <= 0.0f) {
        return;
    }

    if (state->battery > 0.0f) {
        state->battery -= 0.02f * dt_seconds;

        if (state->battery < 0.0f) {
            state->battery = 0.0f;
        }
    }

    if (state->battery < 20.0f) {
        VehicleState_SetStatus(state, "LOW_BATT");
    } else {
        VehicleState_SetStatus(state, "OK");
    }

    if (!state->armed) {
        return;
    }

    switch (state->mode) {
        case MODE_MANUAL:
            // keep simulated vehicle mostly stationary;
            break;
        
        case MODE_AUTO:
            if (state->altitude < 10.0f) {
                state->altitude += 0.5f * dt_seconds;
            }

            state->x += 0.4f * dt_seconds;
            state->y += 0.2f * dt_seconds;

            if (state->x > 2.0f && state->waypoint == 0) {
                state->waypoint = 1;
            } else if (state->x > 4.0f && state->waypoint == 1) {
                state->waypoint = 2;
            } else if (state->x > 6.0f && state->waypoint == 2) {
                state->waypoint = 3;
            }

            break;
        
        case MODE_RTL:
            if (state->altitude > 0.0f && state->x <= 0.0f && state->y <= 0.0f) {
                state->altitude -= 0.5f * dt_seconds;

                if (state->altitude < 0.0f) {
                    state->altitude = 0.0f;
                }
            }

            if (state->x > 0.0f) {
                state->x -= 0.3f * dt_seconds;

                if (state->x < 0.0f) {
                    state->x = 0.0f;
                }
            }

            if (state->y > 0.0f) {
                state->y -= 0.3f * dt_seconds;

                if (state->y < 0.0f) {
                    state->y = 0.0f;
                }
            }

            state->waypoint = 0;
            break;
        
        default:
            VehicleState_SetStatus(state, "BAD_MODE");
            break;
    }
}

void VehicleState_Reset(VehicleState* state) {
    VehicleState_Init(state);
}