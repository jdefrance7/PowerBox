#ifndef BATTERY_H
#define BATTERY_H

// Arduino Library
#include <Arduino.h>

#define VBATPIN A7

#define VBATLOW 3.3

float getBatteryVoltage();

#endif
