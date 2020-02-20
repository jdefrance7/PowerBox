#include "Reading.h"

int readFile()
{
  if(!DATALOGGER_ENABLED || (!SERIAL_ENABLED && !BLUETOOTH_ENABLED))
  {
    broadcast("\nERROR: Required modules not enabled.\n");
    return -1;
  }

  if(printRoot())
  {
    broadcast("\nERROR: Unable to print root contents.\n");
    return -1;
  }

  int filenumber;
  if(getFilenumber(filenumber))
  {
    broadcast("\nERROR: Invalid filenumber.\n");
    return -1;
  }

  File logfile;
  if(getFileFromNumber(filenumber, logfile))
  {
    broadcast("\nERROR: Unable to get file from number.\n");
    return -1;
  }

  if(printFile(logfile))
  {
    broadcast("\nERROR: An error occured while printing the file.\n");
    return -1;
  }

  return 0;
}

// Prints Files in Root as Numbered List
int printRoot()
{
  // Check for Errors
  if(!DATALOGGER_ENABLED)
  {
    broadcast("\nERROR: Datalogger not available.\n");
    return -1;
  }

  // Open Base Directory
  File root = SD.open("/");
  if(!root)
  {
    broadcast("\nERROR: Unable to open root.");
    return -1;
  }

  // Start with First File
  File entry;
  int entryNumber = 1;

  // Iterate through Root Directory, Printing Valid Filenames
  while(1)
  {
    entry = root.openNextFile();

    // End of Root Directory
    if(!entry)
    {
      break;
    }

    // Skip Other Directories
    else if(entry.isDirectory())
    {
      continue;
    }

    // Print Valid File Name and Number
    else
    {
      broadcast(String(entryNumber++)+". "+entry.name()+"\n");
    }
  }
  return 0;
}

// User Selects Filenumber from Root List
int getFileNumber(int &filenumber)
{
  // Ask User for Filenumber
  broadcast("\nINFO: Enter Filenumber: ");

  // Get Filenumber from User
  int number = 0;
  while(number == 0)
  {
    number = getInt();

    // No Entry
    if(number == 0)
    {
      continue;
    }

    // Entry out of Range
    else if(number <= 0 || number >= entryNumber)
    {
      broadcast("\nERROR: Invalid filenumber '"+String(number)+"'.\n");
    }

    // Entry in Range
    else
    {
      break;
    }
  }

  broadcast("\nFILENUMBER: "+String(number)+"\n");

  filenumber = number;

  // Close Root
  root.close();

  return 0;
}

// Returns File Object from Number in Root List
File getFileFromNumber(int filenumber, File &logfile)
{
    // Check for Errors
   if(!DATALOGGER_ENABLED)
   {
     broadcast("\nERROR: Datalogger not available.\n");
     return -1;
   }

   File fileFromNumber;
   File root = SD.open("/");
   if(!root)
   {
     broadcast("\nERROR: Unable to open root.");
     return -1;
   }

   char* filename;
   File entry;
   int entryNumber = 1;

   // Iterate Through Base Directory
   while(1)
   {
     // Open Next File in Directory
     entry = root.openNextFile();

     // Reach End of Root Without Finding File
     if(!entry)
     {
       break;
     }

     // Skip Other Directories
     else if(entry.isDirectory())
     {
       continue;
     }

     // Check if File is Correct
     else
     {
       // Desired File
       if(filenumber == entryNumber)
       {
         broadcast("\nFILENAME: "+String(entry.name())+"\n");

         fileFromNumber = entry;

         break;
       }

       // Try Next File
       else
       {
         entryNumber++;
       }
     }
   }

   // Close Root
   root.close();

   return fileFromNumber;
}

// Prints Contents of File to Available Streams
int printFile(File logfile)
{
  // Check Datalogger
  if(!DATALOGGER_ENABLED)
  {
    broadcast("\nERROR: Datalogger not available.\n");

    return -1;
  }
  if(!logfile)
  {
    broadcast("\nERROR: File not open.\n");
    return -1;
  }

  // Line Variables
  char character;
  String line = "";

  // Read File Line by Line
  while(logfile.available())
  {
    // Read Next Char
    character = logfile.read();

    // Add Character to Line
    line.concat(character);

    // Print Line by Line
    if(character == '\n')
    {
      // Print Line
      broadcast(line);

      // Clear Line
      line = "";
    }
  }

  // Return Success
  return 0;
}
