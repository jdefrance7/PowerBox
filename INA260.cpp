#include "ina260.h"

bool INA260_ENABLED = false;

// Adafruit_INA260 Object
Adafruit_INA260 ina260 = Adafruit_INA260();

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
  ina260.setVoltageConversionTime(VOLTAGE_CONVERSION_TIME);

  // Set current conversion time
  ina260.setCurrentConversionTime(CURRENT_CONVERSION_TIME);

  INA260_ENABLED = true;
  return 0;
}
