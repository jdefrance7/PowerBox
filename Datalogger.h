#ifndef DATALOGGER_H
#define DATALOGGER_H

/*
  Library Wrapper for Arduino SD & Adafruit Datalogger

  Provides:
    - Initialization Function (initDatalogger())
    - Datalogger Status Flag (DATALOGGER_ENABLED)
*/

// Arduino Library
#include <Arduino.h>

// Datalogger Library
#include <SD.h>

// Datalogger Chip Select Pin
#define DATALOGGER_CS   4

// Status Flag
extern bool DATALOGGER_ENABLED;

// Datalogger Initializer
int initDatalogger();

#endif // DATALOGGER_H
