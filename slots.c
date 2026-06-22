#include <string.h>
#include "slots.h"

void initParkingLot(ParkingLot *lot) {
    lot->totalSlots = MAX_SLOTS;
    lot->totalRevenue = 0.0f;
    lot->vehiclesServed = 0;

    // Config: 10 Car, 5 Bike, 3 EV, 2 Handicapped
    for (int i = 0; i < MAX_SLOTS; i++) {
        lot->slots[i].slotID = i + 1;
        lot->slots[i].isOccupied = 0;
        lot->slots[i].plateNumber[0] = '\0';
        lot->slots[i].entryTime = 0;

        if (i < 10) {
            lot->slots[i].allowedType = CAR;
        } else if (i < 15) {
            lot->slots[i].allowedType = BIKE;
        } else if (i < 18) {
            lot->slots[i].allowedType = EV;
        } else {
            lot->slots[i].allowedType = HANDICAPPED;
        }
    }
}

int findVehicleSlotIndex(const ParkingLot *lot, const char *plateNumber) {
    for (int i = 0; i < lot->totalSlots; i++) {
        if (lot->slots[i].isOccupied && strcmp(lot->slots[i].plateNumber, plateNumber) == 0) {
            return i;
        }
    }
    return -1;
}

const char* getVehicleTypeName(VehicleType type) {
    switch (type) {
        case CAR: return "CAR";
        case BIKE: return "BIKE";
        case EV: return "EV";
        case HANDICAPPED: return "HANDICAPPED";
        default: return "UNKNOWN";
    }
}
