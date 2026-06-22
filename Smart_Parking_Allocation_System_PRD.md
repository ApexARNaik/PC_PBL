# Product Requirements Document (PRD)
## Smart Parking Allocation System — C Backend (PBL Project)

---

## 1. Problem Statement

Manual parking management leads to inefficient slot usage, confusion over vehicle types (car/bike/EV/handicapped), no record of who parked where or for how long, and no automated billing. This project builds a **rule-based decision-making backend** in C that automatically allocates parking slots to incoming vehicles, tracks occupancy, calculates fees on exit, and maintains a log — simulating how a real smart-parking system would make decisions.

This falls under **Decision-Making Systems**: given inputs (vehicle type, arrival time, slot availability), the system *decides* the best slot to assign, just like real-world smart parking does.

---

## 2. Objectives

1. Simulate a parking lot with a fixed number of slots, divided into categories (Car, Bike, EV, Handicapped).
2. Automatically allocate the most suitable free slot to an arriving vehicle based on rules (not manual choice).
3. Track which vehicle occupies which slot, and since when.
4. Calculate parking fee automatically when a vehicle exits, based on duration and vehicle type.
5. Persist data to a file so the lot's state survives program restarts (and can later be read by a frontend).
6. Provide a clean, console-menu-driven interface for now, but built so that the **logic layer (decision engine) is fully separate from the I/O layer**, so a frontend (web/desktop) can later sit on top.

---

## 3. Scope

### In Scope
- Fixed-size parking lot (configurable, e.g. 20 slots: 10 Car, 5 Bike, 3 EV, 2 Handicapped).
- Vehicle entry → automatic slot allocation based on rules.
- Vehicle exit → fee calculation + slot freeing.
- Real-time view of slot occupancy (console table).
- Logging every entry/exit to a file (`parking_log.txt`).
- Persisting current slot state to a file (`slots_data.txt`) so the program can resume after restart.
- Basic search: find which slot a given vehicle number is parked in.
- Basic reporting: total revenue, total vehicles served, occupancy percentage.

### Out of Scope (for this PBL, but mentioned as future extensions)
- Real sensors / IoT integration.
- GUI (handled later by your separate frontend layer).
- Multi-floor parking, reservation system, payment gateway integration.
- Concurrency / multi-user simultaneous access (single-process console app only).

---

## 4. System Architecture

The system is structured in **layers**, which is the key design decision that makes future frontend integration easy.

```
┌─────────────────────────────┐
│   Presentation Layer        │  <- console menu (main.c) — THIS gets replaced/wrapped
│   (printf/scanf menu loop)  │     later by your other-language frontend
└──────────────┬───────────────┘
               │ calls
┌──────────────▼───────────────┐
│   Decision / Logic Layer     │  <- allocateSlot(), calculateFee(), findVehicle()
│   (allocation.c, billing.c)  │     PURE FUNCTIONS — no printf inside these!
└──────────────┬───────────────┘
               │ reads/writes
┌──────────────▼───────────────┐
│   Data Layer                 │  <- struct arrays in memory
│   (slots.c)                  │     + file persistence (storage.c)
└───────────────────────────────┘
```

**Why this matters for your team:** Because the Logic Layer doesn't do any printing/scanning itself — it just takes inputs and returns results — your eventual frontend (in Python/JS/whatever) can call into this same logic (via a thin C API, or by re-compiling the same .c files behind a different main()) without rewriting the decision-making rules.

---

## 5. Module Breakdown (for team division)

Recommended split across 4 team members:

| Module | File(s) | Responsibility | Suggested Owner |
|---|---|---|---|
| **Data Structures & Slot Management** | `slots.h`, `slots.c` | Define structs, initialize lot, mark slot occupied/free, search slots | Member 1 |
| **Allocation Decision Engine** | `allocation.c` | The core rule-based logic: which slot to assign to which vehicle | Member 2 |
| **Billing & Reporting** | `billing.c`, `report.c` | Fee calculation, revenue stats, occupancy % | Member 3 |
| **File Persistence & Logging** | `storage.c` | Save/load slot state, write entry/exit logs | Member 3 or 4 |
| **Console Interface (Main)** | `main.c` | Menu loop, calls into other modules, displays tables | Member 4 |

Each `.c` file should have a matching `.h` header so files can be compiled together cleanly:
```
gcc main.c slots.c allocation.c billing.c report.c storage.c -o parking_system
```

---

## 6. Data Structures

```c
// slots.h

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
```

---

## 7. Core Decision Logic (the "Decision-Making" part)

This is the heart of the project — the rules an automated system would use to pick a slot.

### Allocation Rules (in priority order):
1. **Type match first**: A car can only go into a CAR slot, a bike into a BIKE slot, etc. (Handicapped slots can be used by any vehicle type if explicitly requested — common real-world rule.)
2. **Nearest available slot**: Among free slots of the matching type, pick the lowest slot ID (simulates "closest to entrance").
3. **Overflow rule**: If no slot of the exact type is free, check if EV/Handicapped slots can accept it as fallback (configurable — e.g., a car *cannot* use a bike slot, but could use an empty EV slot if no car slots are left and EV slot is currently unreserved).
4. **Full lot rule**: If absolutely no valid slot is available, reject entry and inform the user ("Parking Full").

### Pseudocode:
```
function allocateSlot(lot, vehicleType, plateNumber):
    for each slot in lot.slots:
        if slot.allowedType == vehicleType AND slot.isOccupied == 0:
            assign slot to vehicle
            slot.isOccupied = 1
            slot.plateNumber = plateNumber
            slot.entryTime = current_time()
            return slot.slotID

    // fallback rule
    for each slot in lot.slots:
        if slot.allowedType == EV or HANDICAPPED AND slot.isOccupied == 0 AND fallback_allowed(vehicleType):
            assign slot (same as above)
            return slot.slotID

    return -1  // no slot available
```

### Fee Calculation Logic:
```
function calculateFee(vehicleType, entryTime, exitTime):
    durationInHours = (exitTime - entryTime) / 3600
    roundedHours = ceil(durationInHours)  // partial hour = full hour charged
    if roundedHours == 0: roundedHours = 1  // minimum 1 hour charge

    rate = getRate(vehicleType)   // e.g. Car=20/hr, Bike=10/hr, EV=15/hr
    return roundedHours * rate
```

---

## 8. Functional Requirements

| ID | Requirement | Priority |
|---|---|---|
| FR1 | System shall initialize a parking lot with a fixed configuration of slot types on first run | High |
| FR2 | System shall allow vehicle entry by capturing plate number and vehicle type | High |
| FR3 | System shall auto-allocate a slot per the rules in Section 7, or reject if full | High |
| FR4 | System shall allow vehicle exit by plate number lookup | High |
| FR5 | System shall calculate and display the fee on exit | High |
| FR6 | System shall display current occupancy as a table (slot ID, type, status, plate) | High |
| FR7 | System shall log every entry/exit event with timestamp to `parking_log.txt` | Medium |
| FR8 | System shall save current slot state to `slots_data.txt` on every change | Medium |
| FR9 | System shall load saved state on startup if file exists | Medium |
| FR10 | System shall display summary report: total revenue, vehicles served, occupancy % | Medium |
| FR11 | System shall let user search for a vehicle by plate number to find its slot | Low |

---

## 9. Console Menu Flow (Presentation Layer)

```
===== SMART PARKING SYSTEM =====
1. Park a Vehicle (Entry)
2. Remove a Vehicle (Exit + Billing)
3. View Current Occupancy
4. Search Vehicle by Plate Number
5. View Reports (Revenue / Occupancy)
6. Exit Program
=================================
Enter choice:
```

Sample run-through:
```
Enter choice: 1
Enter vehicle type (1-Car 2-Bike 3-EV 4-Handicapped): 1
Enter plate number: KA01AB1234
✅ Allocated to Slot #3 (CAR) at 10:42 AM

Enter choice: 2
Enter plate number to remove: KA01AB1234
Duration: 2 hrs 15 mins → Charged for 3 hrs
💰 Total Fee: ₹60
Slot #3 is now FREE
```

---

## 10. File Handling Design (Persistence)

Two files:
- **`slots_data.txt`** — overwritten every time slot state changes. Format (one line per slot):
  ```
  slotID,allowedType,isOccupied,plateNumber,entryTime
  1,CAR,1,KA01AB1234,1750580520
  2,CAR,0,,0
  ```
- **`parking_log.txt`** — appended to, never overwritten. Format:
  ```
  [2026-06-22 10:42:00] ENTRY  | Slot 3 | CAR | KA01AB1234
  [2026-06-22 13:00:00] EXIT   | Slot 3 | CAR | KA01AB1234 | Fee: 60
  ```

This means even your *future frontend* — before it's wired into the C logic directly — could read `slots_data.txt` to display live status, as a quick-and-dirty integration path.

---

## 11. Non-Functional Requirements

- Code must compile cleanly with `gcc -Wall` (no warnings) — good practice for grading.
- All modules must be separated into `.c`/`.h` files (not one giant `main.c`) to reflect real software engineering practice and ease team collaboration via Git.
- Input validation required (e.g., reject empty plate numbers, invalid menu choices, prevent duplicate plate entries).
- Should handle at minimum 20 slots without performance concern (trivial for this scale).

---

## 12. Suggested Timeline (for a team, ~3-4 weeks)

| Week | Milestone |
|---|---|
| Week 1 | Finalize requirements, design structs, set up Git repo, each member stubs their module's function signatures |
| Week 2 | Implement Slot Management + Allocation Engine; integrate and test together |
| Week 3 | Implement Billing, Reporting, File Persistence; integrate with menu |
| Week 4 | Full integration testing, edge cases (full lot, invalid input), polish console output, write report/documentation, prepare for frontend hookup |

---

## 13. Testing Plan

| Test Case | Expected Result |
|---|---|
| Park a car when car slots available | Allocated to lowest-ID free car slot |
| Park a car when all car slots full, EV slot free | Allocated to EV slot (if fallback enabled) |
| Park a vehicle when lot completely full | "Parking Full" message, no crash |
| Exit a vehicle that was never parked | "Vehicle not found" message |
| Exit after >24 hrs | Correct multi-day fee calculation |
| Restart program after entries | Previously saved occupancy reloads correctly from file |
| Enter duplicate plate number while already parked | Rejected with appropriate message |

---

## 14. Future Integration Path (Frontend Later)

Since you mentioned the frontend will be built separately later, two practical paths once this C backend is solid:

1. **File-bridge approach (quick)**: Frontend periodically reads `slots_data.txt` to show live status, and writes "command files" that a watcher in the C program picks up. Simple but a bit hacky.
2. **Proper API approach (recommended later)**: Compile the Logic Layer (`allocation.c`, `billing.c`, `slots.c`) as a small server (e.g., wrap with a minimal HTTP layer, or call via subprocess with command-line arguments returning JSON-like output) so any frontend language can call it cleanly. This is why keeping printf/scanf OUT of the logic functions now will save you significant rework later.

---

## 15. Deliverables Checklist for Submission

- [ ] Source code (all `.c`/`.h` files, organized by module)
- [ ] Compiled executable / Makefile
- [ ] Sample `slots_data.txt` and `parking_log.txt` outputs
- [ ] This PRD / design document
- [ ] Short report covering: problem statement, design, screenshots of console runs, individual contribution breakdown per team member
- [ ] (Optional but impressive) A short README explaining how to compile and run
