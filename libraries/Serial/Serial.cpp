#include "Serial.h"

// Serial Initialization
int initSerial()
{
  // Begins serial module
  Serial.begin(BAUDRATE);

  // Checks for success
  if(!Serial)
  {
    return -1;
  }
  return 0;
}
