#ifndef POWERBOX_H
#define POWERBOX_H

/*
  Library to Support Basic Power Box Functionality
*/

#include "BuiltinLED.h"
#include "Logging.h"
#include "Reading.h"

int initModules();
int moduleStatus();
int resetDevice();
int printInfo();

#endif // POWERBOX_H
