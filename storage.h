#ifndef STORAGE_H
#define STORAGE_H

#include "slots.h"

// Saves the current state of the parking lot to slots_data.txt
void saveSlotsData(const ParkingLot *lot);

// Loads the state of the parking lot from slots_data.txt if it exists
// Returns 1 if loaded successfully, 0 otherwise
int loadSlotsData(ParkingLot *lot);

// Logs an entry or exit event to parking_log.txt
void logEvent(const char *eventType, int slotID, VehicleType type, const char *plateNumber, float fee);

#endif // STORAGE_H
