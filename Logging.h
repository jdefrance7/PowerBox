#ifndef LOGGING_H
#define LOGGING_H

/*
  Library to Handle Logging Functionality
*/

// Project Libraries
#include "battery.h"
#include "datalogger.h"
#include "rtc.h"
#include "streams.h"

// Number of Seconds Between Log Entries
#define LOG_INTERVAL 5

// Logs Data for Specified Duration
int logging();

#endif // LOGGING_H
