#ifndef BILLING_H
#define BILLING_H

#include "slots.h"

// Calculate the fee for a given vehicle type and duration
float calculateFee(VehicleType type, long entryTime, long exitTime);

#endif // BILLING_H
