#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "slots.h"
#include "allocation.h"
#include "billing.h"
#include "report.h"
#include "storage.h"

void clearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { }
}

int main(int argc, char *argv[]) {
    ParkingLot lot;
    initParkingLot(&lot);
    loadSlotsData(&lot); // Silently load data

    // --- CLI API MODE ---
    // Example: parking_system --park 0 MH123
    // Example: parking_system --remove MH123
    if (argc > 1) {
        if (strcmp(argv[1], "--park") == 0 && argc == 4) {
            int typeInput = atoi(argv[2]);
            char *plateNumber = argv[3];
            
            if (findVehicleSlotIndex(&lot, plateNumber) != -1) {
                printf("{\"status\": \"error\", \"message\": \"Vehicle is already parked\"}\n");
                return 1;
            }

            int slotID = allocateSlot(&lot, (VehicleType)typeInput, plateNumber, time(NULL));
            if (slotID != -1) {
                lot.vehiclesServed++;
                logEvent("ENTRY", slotID, (VehicleType)typeInput, plateNumber, 0.0f);
                saveSlotsData(&lot);
                printf("{\"status\": \"success\", \"slotID\": %d}\n", slotID);
                return 0;
            } else {
                printf("{\"status\": \"error\", \"message\": \"Parking Full\"}\n");
                return 1;
            }
        } 
        else if (strcmp(argv[1], "--remove") == 0 && argc == 3) {
            char *plateNumber = argv[2];
            int slotIdx = findVehicleSlotIndex(&lot, plateNumber);
            if (slotIdx == -1) {
                printf("{\"status\": \"error\", \"message\": \"Vehicle not found\"}\n");
                return 1;
            }

            ParkingSlot *slot = &lot.slots[slotIdx];
            long exitTime = time(NULL);
            if (exitTime == slot->entryTime) exitTime += 3600; // Testing hack

            float fee = calculateFee(slot->allowedType, slot->entryTime, exitTime);
            lot.totalRevenue += fee;
            logEvent("EXIT", slot->slotID, slot->allowedType, plateNumber, fee);

            int freedSlotId = slot->slotID;

            slot->isOccupied = 0;
            slot->plateNumber[0] = '\0';
            slot->entryTime = 0;
            saveSlotsData(&lot);

            printf("{\"status\": \"success\", \"fee\": %.2f, \"slotID\": %d}\n", fee, freedSlotId);
            return 0;
        }
        else {
            printf("{\"status\": \"error\", \"message\": \"Invalid command\"}\n");
            return 1;
        }
    }

    // --- INTERACTIVE MODE ---
    int choice;
    do {
        printf("\n===== SMART PARKING SYSTEM =====\n");
        printf("1. Park a Vehicle (Entry)\n");
        printf("2. Remove a Vehicle (Exit + Billing)\n");
        printf("3. View Current Occupancy\n");
        printf("4. Search Vehicle by Plate Number\n");
        printf("5. View Reports (Revenue / Occupancy)\n");
        printf("6. Exit Program\n");
        printf("=================================\n");
        printf("Enter choice: ");

        if (scanf("%d", &choice) != 1) {
            clearInputBuffer();
            printf("Invalid input. Please enter a number.\n");
            continue;
        }
        clearInputBuffer();

        if (choice == 1) {
            int typeInput;
            printf("Enter vehicle type (1-Car 2-Bike 3-EV 4-Handicapped): ");
            if (scanf("%d", &typeInput) != 1 || typeInput < 1 || typeInput > 4) {
                clearInputBuffer();
                printf("Invalid vehicle type.\n");
                continue;
            }
            clearInputBuffer();

            VehicleType type = (VehicleType)(typeInput - 1);
            char plateNumber[MAX_PLATE_LEN];
            printf("Enter plate number: ");
            if (fgets(plateNumber, sizeof(plateNumber), stdin)) {
                plateNumber[strcspn(plateNumber, "\n")] = 0; 
            }

            if (strlen(plateNumber) == 0) {
                printf("Plate number cannot be empty.\n");
                continue;
            }

            if (findVehicleSlotIndex(&lot, plateNumber) != -1) {
                printf("Vehicle with plate number %s is already parked.\n", plateNumber);
                continue;
            }

            int slotID = allocateSlot(&lot, type, plateNumber, time(NULL));
            if (slotID != -1) {
                printf("✅ Allocated to Slot #%d (%s)\n", slotID, getVehicleTypeName(type));
                lot.vehiclesServed++;
                logEvent("ENTRY", slotID, type, plateNumber, 0.0f);
                saveSlotsData(&lot);
            } else {
                printf("❌ Parking Full for %s.\n", getVehicleTypeName(type));
            }
        } else if (choice == 2) {
            char plateNumber[MAX_PLATE_LEN];
            printf("Enter plate number to remove: ");
            if (fgets(plateNumber, sizeof(plateNumber), stdin)) {
                plateNumber[strcspn(plateNumber, "\n")] = 0;
            }

            int slotIdx = findVehicleSlotIndex(&lot, plateNumber);
            if (slotIdx == -1) {
                printf("Vehicle not found.\n");
            } else {
                ParkingSlot *slot = &lot.slots[slotIdx];
                long exitTime = time(NULL);
                if (exitTime == slot->entryTime) exitTime += 3600;

                float fee = calculateFee(slot->allowedType, slot->entryTime, exitTime);
                lot.totalRevenue += fee;

                double durationHrs = (double)(exitTime - slot->entryTime) / 3600.0;
                printf("Duration: %.1f hrs\n", durationHrs);
                printf("💰 Total Fee: ₹%.2f\n", fee);
                printf("Slot #%d is now FREE\n", slot->slotID);

                logEvent("EXIT", slot->slotID, slot->allowedType, plateNumber, fee);
                slot->isOccupied = 0;
                slot->plateNumber[0] = '\0';
                slot->entryTime = 0;
                saveSlotsData(&lot);
            }
        } else if (choice == 3) {
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
            printf("Exiting program...\n");
        } else {
            printf("Invalid choice.\n");
        }
    } while (choice != 6);

    return 0;
}
