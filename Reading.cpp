#include "reading.h"

int reading()
{
  // Check required modules
  if(!DATALOGGER_ENABLED || (!SERIAL_ENABLED && !BLUETOOTH_ENABLED))
  {
    broadcast("\nERROR: Required modules not enabled.\n");
    return -1;
  }

  // Print root directory as numbered list
  File root = SD.open("/");
  if(!root)
  {
    broadcast("\nERROR: Unable to open root directory.\n");
    return -1;
  }

  // Start file list with number one
  int entryNumber = 1;

  // Iterate through root and print valid filenames
  File entry;
  while(1)
  {
    // Open next file in root
    entry = root.openNextFile();

    // End of root
    if(!entry) {break;}

    // Skip directories
    else if(entry.isDirectory()) {continue;}

    // Print file name and number
    else
    {
      broadcast(String(entryNumber++)+". "+entry.name()+"\n");
    }
  }

  // Close root
  root.close();

  // Get filenumber from user
  int fileNumber = 0;
  while(fileNumber == 0)
  {
    fileNumber = getInt();
  }

  // Check valid filenumber
  if(fileNumber <= 0 || number >= entryNumber)
  {
    broadcast("\nERROR: Invalid file number.\n");
    return -1;
  }

  // Reset list number
  entryNumber = 1;

  // Iterate through root to find file
  root = SD.open("/");
  if(!root)
  {
    broadcast("\nERROR: Unable to open root directory.\n");
    return -1;
  }

  while(1)
  {
    // Open next file in root
    entry = root.openNextFile();

    // End of root
    if(!entry) {break;}

    // Skip directories
    else if(entry.isDirectory()) {continue;}

    // Check entry number
    else
    {
      // Desired file
      if(entryNumber == fileNumber)
      {
        // Line variables
        char character;
        String line = "";

        // Read file line by line
        while(entry.available())
        {
          // Read in character
          character = logfile.read();

          // Add character to line
          line.concat(character);

          // Line complete
          if(character == '\n')
          {
            // Print Line
            broadcast(line);

            // Clear Line
            line = "";
          }
        }
        break;
      }

      // Keep searching
      else
      {
        entryNumber++;
      }
    }
  }

  // Close entry
  entry.close();
  if(entry)
  {
    broadcast("\WARNING: Unable to close entry.\n");
  }

  // Close Root
  root.close();
  if(root)
  {
    broadcast("\nERROR: Unable to close root.\n");
    return -1;
  }

  return 0;
}
