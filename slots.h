#ifndef SLOTS_H
#define SLOTS_H

#include <time.h>

#define MAX_SLOTS 20
#define MAX_PLATE_LEN 15

typedef enum {
    CAR,
    BIKE,
    EV,
    HANDICAPPED
} VehicleType;

typedef struct {
    int slotID;
    VehicleType allowedType;   // what type of vehicle this slot is reserved for
    int isOccupied;            // 0 = free, 1 = occupied
    char plateNumber[MAX_PLATE_LEN]; // empty string if free
    long entryTime;            // stored as time_t (seconds since epoch)
} ParkingSlot;

typedef struct {
    ParkingSlot slots[MAX_SLOTS];
    int totalSlots;
    float totalRevenue;
    int vehiclesServed;
} ParkingLot;

// Initializes the parking lot with the default configuration
void initParkingLot(ParkingLot *lot);

// Returns the index of the slot occupied by the given plateNumber, or -1 if not found
int findVehicleSlotIndex(const ParkingLot *lot, const char *plateNumber);

// Convert VehicleType enum to string
const char* getVehicleTypeName(VehicleType type);

#endif // SLOTS_H
