#ifndef RTC_H
#define RTC_H

/*
  Library Wrapper for RTC_PCF8523

  Provides:
    - RTC_PCF8523 Object (rtc)
    - RTC Status Flag (RTC_ENABLED)
    - Initialization Function (initRTC())
*/

// Arduino Library
#include <Arduino.h>

// RTC Library
#include <RTClib.h>

// Status Flag
extern bool RTC_ENABLED;

// RTC Object
extern RTC_PCF8523 rtc;

// RTC initializer
int initRTC();

long getTime();

String getTimestamp();

#endif // RTC_H
