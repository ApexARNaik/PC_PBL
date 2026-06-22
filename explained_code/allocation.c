#include "allocation.h"
#include <string.h>

// This function acts as the "Decision Engine". It decides exactly WHICH parking slot a vehicle gets.
int allocateSlot(ParkingLot *lot, VehicleType type, const char *plateNumber, long currentTime) {
    
    // RULE 1: STRICT TYPE MATCHING
    // We loop through all available slots in the parking lot.
    for (int i = 0; i < lot->totalSlots; i++) {
        // If the slot is meant for this specific vehicle type AND it is currently free (isOccupied == 0)
        if (lot->slots[i].allowedType == type && lot->slots[i].isOccupied == 0) {
            
            // Mark the slot as taken
            lot->slots[i].isOccupied = 1;
            
            // Copy the license plate number into the slot's memory securely.
            // strncpy prevents buffer overflows by only copying up to MAX_PLATE_LEN - 1 characters.
            strncpy(lot->slots[i].plateNumber, plateNumber, MAX_PLATE_LEN - 1);
            
            // Ensure the string is null-terminated (a requirement for C strings).
            lot->slots[i].plateNumber[MAX_PLATE_LEN - 1] = '\0';
            
            // Record exactly when the vehicle entered (used later for billing).
            lot->slots[i].entryTime = currentTime;
            
            // Return the ID of the assigned slot so the Presentation layer can show it to the user.
            return lot->slots[i].slotID;
        }
    }

    // RULE 2: FALLBACK (OVERFLOW) RULE
    // If we reach this point, it means no slots of the EXACT required type were available.
    // The PRD specifies that Cars and Bikes can use empty EV slots as a fallback option.
    // Notice we do NOT allow fallback into HANDICAPPED slots (those are strictly reserved).
    if (type == CAR || type == BIKE) {
        // Loop through all slots again
        for (int i = 0; i < lot->totalSlots; i++) {
            // Check if there is a free EV slot
            if (lot->slots[i].allowedType == EV && lot->slots[i].isOccupied == 0) {
                
                // If found, allocate this EV slot to the Car/Bike just like above.
                lot->slots[i].isOccupied = 1;
                strncpy(lot->slots[i].plateNumber, plateNumber, MAX_PLATE_LEN - 1);
                lot->slots[i].plateNumber[MAX_PLATE_LEN - 1] = '\0';
                lot->slots[i].entryTime = currentTime;
                
                return lot->slots[i].slotID;
            }
        }
    }

    // RULE 3: FULL PARKING LOT
    // If we reach here, neither the strict match nor the fallback match worked.
    // We return -1 to signal to the caller (main.c) that allocation failed.
    return -1;
}
