#ifndef INA260_H
#define INA260_H

// Proivdes Adafruit_INA260 object (ina260) and init function.

// Arduino Library
#include <Arduino.h>

// INA260 Library
#include <Adafruit_INA260.h>

// See Adafruit_INA260.h for setting details.

// Averaging Count
#define AVERAGING_COUNT INA260_COUNT_16

// Voltage Conversion Time
#define VOLTAGE_CONVERSION_TIME INA260_TIME_140_us

// Current Conversion Time
#define CURRENT_CONVERSION_TIME INA260_TIME_140_us

// INA260 Object
extern Adafruit_INA260 ina260;

// INA260 Initializer
int initINA260();

#endif // INA260_H
