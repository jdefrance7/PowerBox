/*
  PROJECT NOTES
    Board: Adafruit Feather M0
    Programmer: AVRISP mkII
*/

// Project Library
#include "PowerBox.h"

void setup()
{
  // Initialize Modules
  initModules();
}

void loop()
{
  // User Input Variable
  static int input;

  // Print Main Menu Options
  broadcast("\nMAIN MENU: \n");
  broadcast(" 1. RECORD DATA \n");
  broadcast(" 2. READ FILE \n");
  broadcast(" 3. MODULE STATUS \n");
  broadcast(" 4. RESET DEVICE \n");
  broadcast(" 5. PRINT INFO \n");

  while(1)
  {
    // Get Input
    input = getInt();

    // Received Input
    if(input != 0)
    {
      broadcast("\nRECEIVED: ");
      broadcast(String(input));
      broadcast("\n");
    }

    // No Input
    if(input == 0)
    {
      continue;
    }

    // Record Data
    else if(input == 1)
    {
      logData();
      break;
    }

    // Read File
    else if(input == 2)
    {
      readFile();
      break;
    }

    // Module Status
    else if(input == 3)
    {
      moduleStatus();
      break;
    }

    // Reset Device
    else if(input == 4)
    {
      resetDevice();
      break;
    }

    // Information
    else if(input == 5)
    {
      printInfo();
      break;
    }

    // Invalid Entry
    else
    {
      broadcast("\nERROR: Invalid entry '"+String(input)+"'.\n");
      break;
    }
  }
}
