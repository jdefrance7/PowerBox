#ifndef STREAMS_H
#define STREAMS_H

// Arduino Library
#include <Arduino.h>

// Project Libraries
#include "Serial.h"
#include "bluetooth.h"
#include "datalogger.h"

int getInt();
long getLong();

void broadcast(String msg);
void broadline(String msg);

void logcast(File log, String msg);
void logline(File log, String msg);

#endif // STREAMS_H
