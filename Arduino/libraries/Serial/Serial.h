#ifndef SERIAL_H
#define SERIAL_H

// Provides Serial UART object (Serial) and init function.

// Arduino Library
#include <Arduino.h>

// Serial Baudrate
#define SERIAL_BAUDRATE 115200

// Initialization Timeout (ms)
#define SERIAL_TIMEOUT 1000

// Serial Flag
extern bool SERIAL_ENABLED;

int initSerial();

#endif
