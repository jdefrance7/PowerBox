#ifndef RTC_H
#define RTC_H

// Provides RTC_PCF8523 object (rtc) and init function.

// Arduino Library
#include <Arduino.h>

// RTC Library
#include <RTClib.h>

// RTC Object
extern RTC_PCF8523 rtc;

// RTC Flag
extern bool RTC_ENABLED;

// RTC initializer
int initRTC();

// Timestamp
String timestamp();

#endif // RTC_H
