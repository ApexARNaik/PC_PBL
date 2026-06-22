#include "storage.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define DATA_FILE "slots_data.txt"
#define LOG_FILE "parking_log.txt"

// Saves the entire state of the parking lot to a file so it can survive program restarts.
void saveSlotsData(const ParkingLot *lot) {
    // Open the file in "w" (write) mode. This OVERWRITES the file completely every time.
    FILE *file = fopen(DATA_FILE, "w");
    if (!file) {
        printf("Error: Could not open %s for writing.\n", DATA_FILE);
        return;
    }

    // Write a CSV-style header line
    fprintf(file, "slotID,allowedType,isOccupied,plateNumber,entryTime\n");

    // Loop through every slot and save its specific data to a new line
    for (int i = 0; i < lot->totalSlots; i++) {
        const ParkingSlot *slot = &lot->slots[i];
        fprintf(file, "%d,%d,%d,%s,%ld\n",
                slot->slotID,
                slot->allowedType,
                slot->isOccupied,
                slot->isOccupied ? slot->plateNumber : "", // Only write plate number if occupied
                slot->entryTime);
    }

    // Always close files to prevent memory leaks and corruption
    fclose(file);
}

// Reads the previously saved file when the program boots up to restore the parking lot.
int loadSlotsData(ParkingLot *lot) {
    // Open file in "r" (read) mode
    FILE *file = fopen(DATA_FILE, "r");
    if (!file) {
        return 0; // The file doesn't exist yet (probably first time running), return failure
    }

    char line[256];
    
    // Read the very first line (the header) and just ignore it
    if (!fgets(line, sizeof(line), file)) {
        fclose(file);
        return 0;
    }

    int i = 0;
    // Read the file line by line until the end
    while (fgets(line, sizeof(line), file) && i < MAX_SLOTS) {
        ParkingSlot *slot = &lot->slots[i];
        
        // --- CUSTOM CSV PARSER ---
        // Since C doesn't have a built-in split() function, we manually slice the string by replacing commas with NULL (\0) terminators.
        char *ptr = line;
        
        // 1. Read Slot ID
        char *comma = strchr(ptr, ','); // Find the first comma
        if (comma) { 
            *comma = '\0'; // Replace comma with end-of-string
            slot->slotID = atoi(ptr); // Convert string to integer
            ptr = comma + 1; // Move the pointer past the comma for the next item
        }
        
        // 2. Read Allowed Type
        comma = strchr(ptr, ',');
        if (comma) { *comma = '\0'; slot->allowedType = (VehicleType)atoi(ptr); ptr = comma + 1; }
        
        // 3. Read Occupied Status
        comma = strchr(ptr, ',');
        if (comma) { *comma = '\0'; slot->isOccupied = atoi(ptr); ptr = comma + 1; }
        
        // 4. Read Plate Number
        comma = strchr(ptr, ',');
        if (comma) { 
            *comma = '\0'; 
            strncpy(slot->plateNumber, ptr, MAX_PLATE_LEN - 1);
            slot->plateNumber[MAX_PLATE_LEN - 1] = '\0';
            ptr = comma + 1; 
        }
        
        // 5. Read Entry Time (long integer)
        slot->entryTime = atol(ptr);

        i++; // Move to the next slot in the array
    }

    fclose(file);
    return 1; // Successfully loaded
}

// Appends a new line to the history log book every time a car enters or exits.
void logEvent(const char *eventType, int slotID, VehicleType type, const char *plateNumber, float fee) {
    // "a" mode means append. We add to the bottom without erasing what was there.
    FILE *file = fopen(LOG_FILE, "a");
    if (!file) {
        printf("Error: Could not open %s for appending.\n", LOG_FILE);
        return;
    }

    // Get the current real-world timestamp
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    char timeStr[30];
    strftime(timeStr, 30, "%Y-%m-%d %H:%M:%S", tm_info); // Format it as "YYYY-MM-DD HH:MM:SS"

    // If there is a fee (Exit event), log it with the money amount.
    // Otherwise (Entry event), just log the basic info.
    if (fee > 0.0f) {
        fprintf(file, "[%s] %-6s | Slot %d | %s | %s | Fee: %.2f\n",
                timeStr, eventType, slotID, getVehicleTypeName(type), plateNumber, fee);
    } else {
        fprintf(file, "[%s] %-6s | Slot %d | %s | %s\n",
                timeStr, eventType, slotID, getVehicleTypeName(type), plateNumber);
    }

    fclose(file);
}
