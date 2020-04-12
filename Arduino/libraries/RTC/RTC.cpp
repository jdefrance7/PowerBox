#include "RTC.h"

// RTC_PCF8523 Object
RTC_PCF8523 rtc;

// RTC Flag
bool RTC_ENABLED = false;

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
    // rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); // configuration

    RTC_ENABLED = false;

    return -1; // production (should already be initialized)
  }

  RTC_ENABLED = true;

  return 0;
}

// Timestamp
String timestamp()
{
  if(RTC_ENABLED)
  {
    return rtc.now().timestamp();
  }
  else
  {
    return String(millis())+"ms";
  }
}
