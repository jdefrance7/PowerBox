#ifndef LED_H
#define LED_H

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

#define ERROR_TOGGLE 300

class LED
{
public:
  LED(int pin);
  int init();
  void on();
  void off();
  void toggle();
  void error();
private:
  int _pin;
  bool _state;
};

extern LED led;

int initLED();

#endif // LED_H
