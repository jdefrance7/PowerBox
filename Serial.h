#ifndef SERIAL_H
#define SERIAL_H

/*
  Wrapper Library for Arduino Serial

  Provides:
    - Serial Object (Serial)
    - Serial Status Flag (SERIAL_ENABLED)
    - Initialization Function (initSerial())
*/

// Arduino Library
#include <Arduino.h>

#define SERIAL_TIMEOUT  5000
#define SERIAL_BAUDRATE 115200

extern bool SERIAL_ENABLED;

int initSerial();

#endif
