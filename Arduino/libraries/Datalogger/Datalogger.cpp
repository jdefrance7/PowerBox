#include "Datalogger.h"

// Dataloger Flag
bool DATALOGGER_ENABLED = false;

// Datalogger Initialization
int initDatalogger()
{
  // Set chip select pin of datalogger
  pinMode(DATALOGGER_CS, OUTPUT);

  // Begin communication with datalogger
  if(!SD.begin(DATALOGGER_CS))
  {
    DATALOGGER_ENABLED = false;

    return -1;
  }

  DATALOGGER_ENABLED = false;

  return 0;
}
