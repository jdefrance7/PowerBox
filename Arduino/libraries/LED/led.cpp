#include "led.h"

LED led = LED();

LED::LED()
{
  _pin = DEFAULT_PIN;
}

LED::LED(int pin)
{
  _pin = pin;
}

void LED::setPin(int pin)
{
  _pin = pin;
}

void LED::on()
{
  _state = true;
  digitalWrite(_pin, _state);
}

void LED::off()
{
  _state = false;
  digitalWrite(_pin, _state);
}
void LED::toggle()
{
  _state = !_state;
  digitalWrite(_pin, _state);
}

void LED::error()
{
  toggle(ERROR_TOGGLE_RATE);
}

void LED::toggle(long rate)
{
  static long last = millis();
  if(millis() - last > rate)
  {
    toggle();
    last = millis();
  }
}
