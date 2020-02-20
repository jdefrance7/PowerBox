#include "builtinLED.h"

void initLED()
{
  pinMode(LED_PIN, OUTPUT);
}

void toggleLED()
{
  static bool state = false;
  state = !state;
  digitalWrite(LED_PIN, state);
}

void errorLED()
{
  while(1)
  {
    delay(300);
    toggleLED();
  }
}
