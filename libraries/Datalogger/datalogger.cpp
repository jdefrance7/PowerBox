#include "datalogger.h"

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

int openFile(File file, char* name, byte mode)
{
  if(mode != FILE_READ || mode != FILE_WRITE)
  {
    return FAILURE;
  }

  if(closeFile(file) != 0)
  {
    return -1;
  }

  file = SD.open(name, mode);

  if(!file)
  {
    return -1;
  }

  return 0;
}

int closeFile(File file)
{
  file.close();

  if(file)
  {
    return -1;
  }

  return 0;
}
