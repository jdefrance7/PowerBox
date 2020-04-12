#include "INA260.h"

// Adafruit_INA260 Object
Adafruit_INA260 ina260 = Adafruit_INA260();

// INA260 Flag
bool INA260_ENABLED = false;

// Adafruit_INA260 Initialization
int initINA260()
{
  // Attempt communication with INA260
  if(!ina260.begin())
  {
    INA260_ENABLED = false;

    return -1;
  }

  // Set averaging count
  ina260.setAveragingCount(AVERAGING_COUNT);

  // Set voltage conversion time
  ina260.setVoltageConversionTime(INA260_TIME_140_us);

  // Set current conversion time
  ina260.setCurrentConversionTime(INA260_TIME_140_us);

  INA260_ENABLED = true;

  return 0;
}
