#ifndef POWERBOX_H
#define POWERBOX_H

/*
  Library to Support Basic Power Box Functionality
*/

#include "led.h"
#include "logging.h"
#include "reading.h"

int initModules();
int moduleStatus();
int resetDevice();
int printInfo();

#endif // POWERBOX_H
