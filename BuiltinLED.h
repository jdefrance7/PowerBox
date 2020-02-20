#ifndef BUILTINLED_H
#define BUILTINLED_H

/*
  Library to interface with the builtin LED onboard.

  Provides:
    - Initialization Function for Builtin LED (initLED())
    - Function to Toggle LED State (toggleLed())
    - Function to Continuously Toggle LED (errorLED())
*/

// Arduino Library
#include <Arduino.h>

// Built-In LED Pin for Adafruit M0
#define LED_PIN 13

void initLED();

void toggleLED();

void errorLED();

#endif // BUILTINLED_H
