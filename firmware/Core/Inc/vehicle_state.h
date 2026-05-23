#ifndef VEHICLE_STATUS_H
#define VEHICLE_STATUS_H

#include <string.h>

typedef enum {
    MODE_MANUAL,
    MODE_AUTO,
    MODE_RTL
} VehicleMode;

typedef struct {
    int armed;
    VehicleMode mode;
    float battery;
    float altitude;
    float x;
    float y;
    int waypoint;
    char status[16];
} VehicleState;


void VehicleState_Init(VehicleState* state);
void VehicleState_Update(VehicleState* state, float dt_seconds);
void VehicleState_Reset(VehicleState* state);

#endif