#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "slots.h"
#include "allocation.h"
#include "billing.h"
#include "report.h"
#include "storage.h"

// A helper function to flush out unwanted trailing characters (like newlines) after someone types a number
void clearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { }
}

int main() {
    // 1. Memory Setup
    // Create the master 'lot' object in memory that holds everything.
    ParkingLot lot;
    initParkingLot(&lot);

    // 2. File Persistence Check
    // Attempt to load the parking lot state from our saved text file.
    if (loadSlotsData(&lot)) {
        printf("Loaded previously saved parking lot state.\n");
    } else {
        printf("Initialized new parking lot.\n"); // No save file found, starting fresh
    }

    int choice;
    // 3. The Main Program Loop (Presentation Layer)
    // This loop runs forever until the user explicitly picks 'Option 6' to exit.
    do {
        // Display the interactive menu
        printf("\n===== SMART PARKING SYSTEM =====\n");
        printf("1. Park a Vehicle (Entry)\n");
        printf("2. Remove a Vehicle (Exit + Billing)\n");
        printf("3. View Current Occupancy\n");
        printf("4. Search Vehicle by Plate Number\n");
        printf("5. View Reports (Revenue / Occupancy)\n");
        printf("6. Exit Program\n");
        printf("=================================\n");
        printf("Enter choice: ");

        // Read the user's menu choice
        if (scanf("%d", &choice) != 1) {
            clearInputBuffer();
            printf("Invalid input. Please enter a number.\n");
            continue; // Skip the rest of the loop and show the menu again
        }
        clearInputBuffer();

        // ---------------------------------------------------------
        // OPTION 1: PARK A VEHICLE
        // ---------------------------------------------------------
        if (choice == 1) {
            int typeInput;
            printf("Enter vehicle type (1-Car 2-Bike 3-EV 4-Handicapped): ");
            if (scanf("%d", &typeInput) != 1 || typeInput < 1 || typeInput > 4) {
                clearInputBuffer();
                printf("Invalid vehicle type.\n");
                continue;
            }
            clearInputBuffer();

            // Convert raw input (1-4) into our internal enum type (0-3)
            VehicleType type = (VehicleType)(typeInput - 1);

            char plateNumber[MAX_PLATE_LEN];
            printf("Enter plate number: ");
            // Safely read string input (including spaces if any)
            if (fgets(plateNumber, sizeof(plateNumber), stdin)) {
                // Remove the hidden newline character \n that fgets captures
                plateNumber[strcspn(plateNumber, "\n")] = 0; 
            }

            // Input Validation
            if (strlen(plateNumber) == 0) {
                printf("Plate number cannot be empty.\n");
                continue;
            }

            // Ensure the exact same license plate isn't ALREADY inside the parking lot
            if (findVehicleSlotIndex(&lot, plateNumber) != -1) {
                printf("Vehicle with plate number %s is already parked.\n", plateNumber);
                continue;
            }

            // Call the Logic Layer to actually do the hard work of finding a valid slot
            int slotID = allocateSlot(&lot, type, plateNumber, time(NULL));
            
            // If allocation succeeded...
            if (slotID != -1) {
                printf("✅ Allocated to Slot #%d (%s)\n", slotID, getVehicleTypeName(type));
                lot.vehiclesServed++;
                
                // Write this entry out to the permanent files immediately
                logEvent("ENTRY", slotID, type, plateNumber, 0.0f);
                saveSlotsData(&lot);
            } else {
                printf("❌ Parking Full for %s.\n", getVehicleTypeName(type));
            }
        } 
        
        // ---------------------------------------------------------
        // OPTION 2: REMOVE A VEHICLE & BILLING
        // ---------------------------------------------------------
        else if (choice == 2) {
            char plateNumber[MAX_PLATE_LEN];
            printf("Enter plate number to remove: ");
            if (fgets(plateNumber, sizeof(plateNumber), stdin)) {
                plateNumber[strcspn(plateNumber, "\n")] = 0;
            }

            // Locate exactly where this car is parked
            int slotIdx = findVehicleSlotIndex(&lot, plateNumber);
            if (slotIdx == -1) {
                printf("Vehicle not found.\n"); // Not currently in the lot
            } else {
                // We found the car. Grab a pointer to its exact memory location.
                ParkingSlot *slot = &lot.slots[slotIdx];
                long exitTime = time(NULL);
                
                // --- DEBUG/TESTING HACK ---
                // If you enter and exit in the same second, the fee would be 0.
                // For testing purposes, we forcefully simulate 1 hour passing so you can see the math work.
                if (exitTime == slot->entryTime) {
                    exitTime += 3600; // Fast forward 1 hour (3600 seconds)
                }

                // Call the Logic Layer to run the complex billing math
                float fee = calculateFee(slot->allowedType, slot->entryTime, exitTime);
                lot.totalRevenue += fee; // Add this specific fee to the lot's overall vault

                // Calculate duration just for displaying nicely to the screen
                double durationHrs = (double)(exitTime - slot->entryTime) / 3600.0;
                printf("Duration: %.1f hrs\n", durationHrs);
                printf("💰 Total Fee: ₹%.2f\n", fee);
                printf("Slot #%d is now FREE\n", slot->slotID);

                // Write the exit details to the permanent file history
                logEvent("EXIT", slot->slotID, slot->allowedType, plateNumber, fee);

                // "Erase" the slot back to default empty state
                slot->isOccupied = 0;
                slot->plateNumber[0] = '\0';
                slot->entryTime = 0;

                // Sync these changes to disk
                saveSlotsData(&lot);
            }
        } 
        // ---------------------------------------------------------
        // OTHER OPTIONS: VIEWING & REPORTING
        // ---------------------------------------------------------
        else if (choice == 3) {
            displayOccupancy(&lot);
        } else if (choice == 4) {
            char plateNumber[MAX_PLATE_LEN];
            printf("Enter plate number to search: ");
            if (fgets(plateNumber, sizeof(plateNumber), stdin)) {
                plateNumber[strcspn(plateNumber, "\n")] = 0;
            }

            int slotIdx = findVehicleSlotIndex(&lot, plateNumber);
            if (slotIdx != -1) {
                printf("Vehicle %s is parked in Slot #%d (%s)\n", 
                       plateNumber, lot.slots[slotIdx].slotID, getVehicleTypeName(lot.slots[slotIdx].allowedType));
            } else {
                printf("Vehicle not found.\n");
            }
        } else if (choice == 5) {
            displaySummaryReport(&lot);
        } else if (choice == 6) {
            printf("Exiting program...\n"); // Breaks the loop and shuts down
        } else {
            printf("Invalid choice.\n");
        }
    } while (choice != 6); // Loop checks if user wanted to exit

    return 0; // Signals the Operating System that the C program shut down cleanly
}
