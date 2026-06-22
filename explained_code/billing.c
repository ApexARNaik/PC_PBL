#include "billing.h"
#include <math.h>

// This function calculates how much a vehicle owes when it exits.
float calculateFee(VehicleType type, long entryTime, long exitTime) {
    
    // Step 1: Calculate duration. 
    // We subtract entry time from exit time to get the duration in seconds.
    // Dividing by 3600.0 converts those seconds into hours.
    double durationInHours = (double)(exitTime - entryTime) / 3600.0;
    
    // Step 2: Round up.
    // Parking lots usually charge for full hours. So 1 hour and 15 minutes is charged as 2 hours.
    // The ceil() function rounds fractions up to the next highest integer.
    int roundedHours = (int)ceil(durationInHours);
    
    // Step 3: Minimum charge.
    // Even if you park for 5 seconds, you pay for at least 1 hour.
    if (roundedHours <= 0) {
        roundedHours = 1; 
    }

    // Step 4: Determine the rate per hour based on the vehicle type.
    float rate = 0.0f;
    switch (type) {
        case CAR: 
            rate = 20.0f; // Cars pay Rs. 20 per hour
            break;
        case BIKE: 
            rate = 10.0f; // Bikes pay Rs. 10 per hour
            break;
        case EV: 
            rate = 15.0f; // Electric vehicles pay Rs. 15 per hour
            break;
        case HANDICAPPED: 
            rate = 10.0f; // Handicapped spots get a discounted rate of Rs. 10 per hour
            break;
        default: 
            rate = 20.0f; // Fallback default rate
            break;
    }

    // Step 5: Final calculation.
    // Multiply the number of billable hours by the hourly rate to get the final fee.
    return roundedHours * rate;
}
