#include "led.h"

LED led = LED(LED_PIN);

LED::LED(int pin)
{
  _pin = pin;
  _state = false;
}

int LED::init()
{
  if(_pin < 0)
  {
    return -1;
  }

  pinMode(_pin, OUTPUT);

  digitalWrite(_pin, _state);

  return 0;
}

void LED::on()
{
  _state = true;
  digitalWrite(_pin, _state);
  return;
}

void LED::off()
{
  _state = false;
  digitalWrite(_pin, _state);
  return;
}

void LED::toggle()
{
  _state = !_state;
  digitalWrite(_pin, _state);
  return;
}

void LED::error()
{
  while(1)
  {
    toggle();
    delay(ERROR_TOGGLE);
  }
}

int initLED()
{
    return led.init();
}
