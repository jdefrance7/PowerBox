/*
  PROJECT NOTES
    Board: Adafruit Feather M0
    Programmer: AVRISP mkII
*/

// Project Library
#include "PowerBox.h"

void setup()
{
  // Initialize modules
  initModules();
}

void loop()
{
  // User input variable
  static int input = 0;

  // Print main menu options
  broadcast("\nMAIN MENU: \n");
  broadcast(" 1. RECORD DATA \n");
  broadcast(" 2. READ FILE \n");
  broadcast(" 3. MODULE STATUS \n");
  broadcast(" 4. RESET DEVICE \n");
  broadcast(" 5. PRINT INFO \n");

  // Get user input
  while(input == 0)
  {
    input = getInt();
  }

  // Print received input
  broadcast("\nINFO: Received input '"+String(input)+"'.\n");

  // Logging
  if(input == 1)
  {
    logging();
  }

  // Reading
  else if(input == 2)
  {
    reading();
  }

  // Module Status
  else if(input == 3)
  {
    moduleStatus();
  }

  // Reset Device
  else if(input == 4)
  {
    resetDevice();
  }

  // Information
  else if(input == 5)
  {
    printInfo();
  }

  // Invalid Entry
  else
  {
    broadcast("\nERROR: Invalid entry '"+String(input)+"'.\n");
  }
}
