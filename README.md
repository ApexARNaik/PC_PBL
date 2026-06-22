# Smart Parking Allocation System - C Backend

This is the C backend for a Smart Parking Allocation System as specified in the PRD.

## Features
- Automatic rule-based slot allocation (Type matching and fallback).
- Slot occupancy tracking.
- Automated billing calculation on exit based on time.
- File-based persistence (`slots_data.txt`).
- Logging of events (`parking_log.txt`).

## Modules
- `slots`: Core data structures and slot management.
- `allocation`: Decision engine for assigning slots.
- `billing`: Fee calculation.
- `report`: Current occupancy and summary reports.
- `storage`: Saving/loading from files.
- `main`: Console interface (Presentation layer).

## Compilation
Run `make` to compile the project:
```sh
make
```

## Running
Execute the compiled binary:
```sh
./parking_system
```
(On Windows: `parking_system.exe`)
