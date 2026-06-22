#include "billing.h"
#include <math.h>

float calculateFee(VehicleType type, long entryTime, long exitTime) {
    double durationInHours = (double)(exitTime - entryTime) / 3600.0;
    int roundedHours = (int)ceil(durationInHours);
    if (roundedHours <= 0) {
        roundedHours = 1; // minimum 1 hour charge
    }

    float rate = 0.0f;
    switch (type) {
        case CAR: rate = 20.0f; break;
        case BIKE: rate = 10.0f; break;
        case EV: rate = 15.0f; break;
        case HANDICAPPED: rate = 10.0f; break; // Discounted rate
        default: rate = 20.0f; break;
    }

    return roundedHours * rate;
}
