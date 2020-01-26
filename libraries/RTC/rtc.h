#ifndef RTC_H
#define RTC_H

// Provides RTC_PCF8523 object (rtc) and init function.

// Arduino Library
#include <Arduino.h>

// RTC Library
#include <RTClib.h>

// RTC Object
extern RTC_PCF8523 rtc;

// RTC initializer
int initRTC();

#endif // RTC_H
