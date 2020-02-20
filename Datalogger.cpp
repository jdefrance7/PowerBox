#include "datalogger.h"

// Status Flag
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

  // Attempt to open root directory
  File root = SD.open("/");
  if(!root)
  {
    DATALOGGER_ENABLED = false;
    return -1;
  }
  root.close();

  DATALOGGER_ENABLED = true;
  return 0;
}
