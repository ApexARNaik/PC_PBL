#include "allocation.h"
#include <string.h>

int allocateSlot(ParkingLot *lot, VehicleType type, const char *plateNumber, long currentTime) {
    // 1. Type match first
    for (int i = 0; i < lot->totalSlots; i++) {
        if (lot->slots[i].allowedType == type && lot->slots[i].isOccupied == 0) {
            lot->slots[i].isOccupied = 1;
            strncpy(lot->slots[i].plateNumber, plateNumber, MAX_PLATE_LEN - 1);
            lot->slots[i].plateNumber[MAX_PLATE_LEN - 1] = '\0';
            lot->slots[i].entryTime = currentTime;
            return lot->slots[i].slotID;
        }
    }

    // 2. Fallback rule: If no slot of exact type, check EV slots
    // Note: Handicapped slots are not used as fallback.
    if (type == CAR || type == BIKE) {
        for (int i = 0; i < lot->totalSlots; i++) {
            if (lot->slots[i].allowedType == EV && lot->slots[i].isOccupied == 0) {
                lot->slots[i].isOccupied = 1;
                strncpy(lot->slots[i].plateNumber, plateNumber, MAX_PLATE_LEN - 1);
                lot->slots[i].plateNumber[MAX_PLATE_LEN - 1] = '\0';
                lot->slots[i].entryTime = currentTime;
                return lot->slots[i].slotID;
            }
        }
    }

    // 3. Full lot rule: no slot available
    return -1;
}
