#include "storage.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define DATA_FILE "slots_data.txt"
#define LOG_FILE "parking_log.txt"

void saveSlotsData(const ParkingLot *lot) {
    FILE *file = fopen(DATA_FILE, "w");
    if (!file) {
        printf("Error: Could not open %s for writing.\n", DATA_FILE);
        return;
    }

    fprintf(file, "slotID,allowedType,isOccupied,plateNumber,entryTime\n");

    for (int i = 0; i < lot->totalSlots; i++) {
        const ParkingSlot *slot = &lot->slots[i];
        fprintf(file, "%d,%d,%d,%s,%ld\n",
                slot->slotID,
                slot->allowedType,
                slot->isOccupied,
                slot->isOccupied ? slot->plateNumber : "",
                slot->entryTime);
    }

    fclose(file);
}

int loadSlotsData(ParkingLot *lot) {
    FILE *file = fopen(DATA_FILE, "r");
    if (!file) {
        return 0; // File does not exist
    }

    char line[256];
    if (!fgets(line, sizeof(line), file)) {
        fclose(file);
        return 0;
    }

    int i = 0;
    while (fgets(line, sizeof(line), file) && i < MAX_SLOTS) {
        ParkingSlot *slot = &lot->slots[i];
        
        char *ptr = line;
        char *comma = strchr(ptr, ',');
        if (comma) { *comma = '\0'; slot->slotID = atoi(ptr); ptr = comma + 1; }
        
        comma = strchr(ptr, ',');
        if (comma) { *comma = '\0'; slot->allowedType = (VehicleType)atoi(ptr); ptr = comma + 1; }
        
        comma = strchr(ptr, ',');
        if (comma) { *comma = '\0'; slot->isOccupied = atoi(ptr); ptr = comma + 1; }
        
        comma = strchr(ptr, ',');
        if (comma) { 
            *comma = '\0'; 
            strncpy(slot->plateNumber, ptr, MAX_PLATE_LEN - 1);
            slot->plateNumber[MAX_PLATE_LEN - 1] = '\0';
            ptr = comma + 1; 
        }
        
        slot->entryTime = atol(ptr);

        i++;
    }

    fclose(file);
    return 1;
}

void logEvent(const char *eventType, int slotID, VehicleType type, const char *plateNumber, float fee) {
    FILE *file = fopen(LOG_FILE, "a");
    if (!file) {
        printf("Error: Could not open %s for appending.\n", LOG_FILE);
        return;
    }

    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    char timeStr[30];
    strftime(timeStr, 30, "%Y-%m-%d %H:%M:%S", tm_info);

    if (fee > 0.0f) {
        fprintf(file, "[%s] %-6s | Slot %d | %s | %s | Fee: %.2f\n",
                timeStr, eventType, slotID, getVehicleTypeName(type), plateNumber, fee);
    } else {
        fprintf(file, "[%s] %-6s | Slot %d | %s | %s\n",
                timeStr, eventType, slotID, getVehicleTypeName(type), plateNumber);
    }

    fclose(file);
}
