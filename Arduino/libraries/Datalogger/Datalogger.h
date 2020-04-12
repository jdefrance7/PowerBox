#ifndef DATALOGGER_H
#define DATALOGGER_H

// Provides datalogger object (SD) and init function.

// Arduino Library
#include <Arduino.h>

// Datalogger Library
#include <SD.h>

// Datalogger Chip Select Pin
#define DATALOGGER_CS   4

// Datalogger Initializer
int initDatalogger();

#endif // DATALOGGER_H
