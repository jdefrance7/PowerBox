#include "Datalogger.h"

// Datalogger Initialization
int initDatalogger()
{
  // Set chip select pin of datalogger
  pinMode(DATALOGGER_CS, OUTPUT);

  // Begin communication with datalogger
  if(!SD.begin(DATALOGGER_CS))
  {
    return -1;
  }

  return 0;
}
