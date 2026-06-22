#include <string.h>
#include "slots.h"

// This function sets up the parking lot when the program first starts.
void initParkingLot(ParkingLot *lot) {
    // Set the overall lot properties
    lot->totalSlots = MAX_SLOTS;   // Currently 20 slots as defined in slots.h
    lot->totalRevenue = 0.0f;      // Starts with zero revenue earned
    lot->vehiclesServed = 0;       // Starts with zero vehicles processed

    // Now, configure each individual slot one by one.
    // According to the PRD: 10 Car, 5 Bike, 3 EV, 2 Handicapped.
    for (int i = 0; i < MAX_SLOTS; i++) {
        lot->slots[i].slotID = i + 1;           // Slots are numbered 1 through 20
        lot->slots[i].isOccupied = 0;           // 0 means it is currently free
        lot->slots[i].plateNumber[0] = '\0';    // Empty string since no vehicle is parked here yet
        lot->slots[i].entryTime = 0;            // No entry time

        // Assigning types based on the slot index
        if (i < 10) {
            // Indices 0 to 9 (10 slots) -> Cars
            lot->slots[i].allowedType = CAR;
        } else if (i < 15) {
            // Indices 10 to 14 (5 slots) -> Bikes
            lot->slots[i].allowedType = BIKE;
        } else if (i < 18) {
            // Indices 15 to 17 (3 slots) -> EVs
            lot->slots[i].allowedType = EV;
        } else {
            // Indices 18 and 19 (2 slots) -> Handicapped
            lot->slots[i].allowedType = HANDICAPPED;
        }
    }
}

// This function acts like a search engine to find where a specific vehicle is parked.
int findVehicleSlotIndex(const ParkingLot *lot, const char *plateNumber) {
    // Loop through every slot in the lot
    for (int i = 0; i < lot->totalSlots; i++) {
        // If the slot is occupied AND the license plate matches the one we're looking for...
        // Note: strcmp compares two strings and returns 0 if they are identical.
        if (lot->slots[i].isOccupied && strcmp(lot->slots[i].plateNumber, plateNumber) == 0) {
            return i; // Return the exact array index where this car is stored
        }
    }
    return -1; // If we finish the loop and find nothing, return -1 (Not Found)
}

// A simple helper function to convert the internal Enum (0, 1, 2, 3) into readable text.
const char* getVehicleTypeName(VehicleType type) {
    switch (type) {
        case CAR: return "CAR";
        case BIKE: return "BIKE";
        case EV: return "EV";
        case HANDICAPPED: return "HANDICAPPED";
        default: return "UNKNOWN";
    }
}
