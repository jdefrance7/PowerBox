#include "Serial.h"

// Status Flag
bool SERIAL_ENABLED = false;

// Serial Initialization
int initSerial()
{
  // Begins serial module
  Serial.begin(SERIAL_BAUDRATE);

  // Checks for success
  long timeout = millis();
  while(!Serial)
  {
    if(millis()-timeout > SERIAL_TIMEOUT)
    {
      SERIAL_ENABLED = false;
      return -1;
    }
  }

  SERIAL_ENABLED = true;
  return 0;
}
