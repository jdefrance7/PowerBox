#ifndef STREAMS_H
#define STREAMS_H

// Arduino Library
#include <Arduino.h>

// Project Libraries
#include "Serial.h"
#include "ble.h"
#include "datalogger.h"

int getInt();
long getLong();

void broadcast(String msg);
void broadline(String msg);

#endif // STREAMS_H
