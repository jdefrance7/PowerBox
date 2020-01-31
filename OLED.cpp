#include "OLED.h"

Adafruit_FeatherOLED oled = Adafruit_FeatherOLED();

int initOLED()
{
  oled.init();
  oled.setBatteryVisible(false);
  return 0;
}
