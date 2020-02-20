#ifndef LOGGING_H
#define LOGGING_H

/*
  Library to Handle Logging Functionality
*/

// Project Libraries
#include "Battery.h"
#include "Datalogger.h"
#include "RTC.h"
#include "Streams.h"

// Number of Seconds Between Log Entries
#define LOG_INTERVAL 5

// Uses Supporting Functions to Record Power Data for Specified Duration
int logData();

// Gets Desired Duration from User
int getDuration(long &duration);

// Attempts to Open Logfile
int openLogfile(File &log);

// Logs Data to Logfile and Streams for Duration
int recordData(long duration, File &log);

// Attempts to Close Logfile
int closeLogfile(File &log);

// Logs Message to Logfile & Streams
int logcast(File log, String msg);

// Logs Message+\n to Logfile & Streams
int logline(File log, String msg);

#endif // LOGGING_H
