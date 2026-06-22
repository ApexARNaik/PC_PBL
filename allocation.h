#ifndef ALLOCATION_H
#define ALLOCATION_H

#include "slots.h"

// Allocates a slot for the given vehicle type and plate number.
// Returns the slot ID if successful, or -1 if no slot is available.
int allocateSlot(ParkingLot *lot, VehicleType type, const char *plateNumber, long currentTime);

#endif // ALLOCATION_H
