#ifndef NATIVE_TESTING
// Mocking file for scd40.h until we have the actual sensor driver coded (if we do it)

#include "scd40.h"

int scd40_read()
{
    int sensorValue = 400; // Placeholder value, replace with actual sensor reading

    return sensorValue;
}
#endif