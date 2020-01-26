#include "rtc.h"

// RTC_PCF8523 Object
RTC_PCF8523 rtc;

// RTC_PCF8523 Initialization
int initRTC()
{
  // Attempt communication with RTC_PCF8523
  if(!rtc.begin())
  {
    return -1;
  }

  // Adjust date/time if not already set
  if(!rtc.initialized())
  {
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  return 0;
}
