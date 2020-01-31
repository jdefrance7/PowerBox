#include "Serial.h"

// Serial Initialization
int initSerial()
{
  // Begins serial module
  Serial.begin(115200);

  // Checks for success
  long timeout = millis();
  while(!Serial)
  {
    if(millis()-timeout > 10000)
    {
      return -1;
    }
  }
  return 0;
}
