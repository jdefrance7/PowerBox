#ifndef READING_H
#define READING_H

/*
  Library to Handle File Reading Functions
*/

// Project Libraries
#include "Datalogger.h"
#include "Streams.h"

// Uses Supporting Functions to Print User Specified File to Streams
int readFile();

// Prints Files in Root as Numbered List
int printRoot();

// User Selects Filenumber from Root List
int getFileNumber(int &filenumber);

// Returns File Object from Number in Root List
File getFileFromNumber(int filenumber, File &logfile);

// Prints Contents of File to Available Streams
int printFile(File logfile);

#endif
