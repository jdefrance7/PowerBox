#ifndef CUSTOM_LED
#define CUSTOM_LED

// Provides custom LED object (led) and support functions.

#include <Arduino.h>

// LED pin used by default constructor
#define DEFAULT_PIN   13

// Toggle rate for led.error()
#define ERROR_TOGGLE_RATE 300

// Basic LED class for hardware debugging
class LED
{
public:

  // Default constructor, sets pin to DEFAULT_PIN
  LED();

  // Constructor with pin declaration
  LED(int pin);

  // Set LED pin
  void setPin(int pin);

  // Turn LED on
  void on();

  // Turn LED off
  void off();

  // Toggle LED state
  void toggle();

  // Error LED state
  void error();

  // Toggle LED state if elapsed time since last call is greater than rate (ms)
  void toggle(long rate);

private:

  // LED pin
  int _pin;

  // LED state
  bool _state;
};

extern LED led;

#endif // CUSTOM_LED
