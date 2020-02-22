#ifndef LOGGING_H
#define LOGGING_H

/*
  Library to Handle Logging Functionality
*/

// Project Libraries
#include "battery.h"
#include "streams.h"
#include "timing.h"
#include "ina260.h"

// Number of Seconds Between Log Entries
#define LOG_INTERVAL 5

// Logs Data for Specified Duration
int logging();

#endif // LOGGING_H
