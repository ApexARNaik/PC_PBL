#ifndef REPORT_H
#define REPORT_H

#include "slots.h"

// Displays the current occupancy of the parking lot
void displayOccupancy(const ParkingLot *lot);

// Displays the summary report (revenue, vehicles served, occupancy percentage)
void displaySummaryReport(const ParkingLot *lot);

#endif // REPORT_H
