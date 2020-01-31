#ifndef OLED_H
#define OLED_H

// Arduino Library
#include <Arduino.h>

// Supporting Libraries
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Adafruit Feather OLED Library
#include <Adafruit_FeatherOLED.h>

extern Adafruit_FeatherOLED oled;

int initOLED();

#endif // OLED_H
