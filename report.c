#include "report.h"
#include <stdio.h>

void displayOccupancy(const ParkingLot *lot) {
    printf("\n--- Current Occupancy ---\n");
    printf("%-7s | %-12s | %-10s | %-15s\n", "Slot ID", "Type", "Status", "Plate Number");
    printf("------------------------------------------------------\n");

    for (int i = 0; i < lot->totalSlots; i++) {
        const ParkingSlot *slot = &lot->slots[i];
        printf("%-7d | %-12s | %-10s | %-15s\n",
               slot->slotID,
               getVehicleTypeName(slot->allowedType),
               slot->isOccupied ? "Occupied" : "Free",
               slot->isOccupied ? slot->plateNumber : "-");
    }
    printf("------------------------------------------------------\n");
}

void displaySummaryReport(const ParkingLot *lot) {
    int occupiedCount = 0;
    for (int i = 0; i < lot->totalSlots; i++) {
        if (lot->slots[i].isOccupied) {
            occupiedCount++;
        }
    }

    float occupancyPercentage = (float)occupiedCount / lot->totalSlots * 100.0f;

    printf("\n--- Summary Report ---\n");
    printf("Total Revenue      : Rs. %.2f\n", lot->totalRevenue);
    printf("Vehicles Served    : %d\n", lot->vehiclesServed);
    printf("Current Occupancy  : %.2f%%\n", occupancyPercentage);
    printf("----------------------\n");
}
