#include "Logging.h"

int logData()
{
  if(!INA260_ENABLED || (!DATALOGGER_ENABLED && !SERIAL_ENABLED))
  {
    broadcast("\nERROR: Required modules not enabled.\n");
    return -1;
  }

  long duration;
  if(getDuration(duration))
  {
    broadcast("\nERROR: Invalid duration.\n");
    return -1;
  }

  File log;
  if(openLogfile(log))
  {
    broadcast("\nERROR: Unable to open logfile.\n");
    return -1;
  }

  if(recordData(duration, log))
  {
    broadcast("\nERROR: Error recording data.\n");
    return -1;
  }

  if(closeLogfile(log))
  {
    broadcast("\nERROR: Unable to close logfile.\n");
    return -1;
  }

  return 0;
}

// Get Duration for Logging Data from User
int getDuration(long &duration)
{
  broadcast("\nINFO: Enter Duration in Seconds\n");

  long value = 0;
  while(value <= 0)
  {
    value = getLong();
    if(value == -1)
    {
      broadcast("\nERROR: Invalid duration '-1'.\n");
      broadcast("\nINFO: Enter Duration in Seconds\n");
    }
  }
  duration = value;

  broadcast("\nRECEIVED: "+String(duration)+"\n");

  return 0;
}

// Attempts to Open Logfile
int openLogfile(File &log)
{
  if(DATALOGGER_ENABLED)
  {
    // Filename Variables
    String filename;
    char charName[16];

    // Find Next Available Filename
    for(int n = 0; n < 1000; n++)
    {
      filename = "LOG_";
      filename.concat(n);
      filename += ".txt";

      // Convert String to Char Array
      filename.toCharArray(charName, 16);

      if(!SD.exists(charName));
      {
        break;
      }
    }

    // Open File
    log = SD.open(charName, FILE_WRITE);
    if(!log)
    {
      // Return Error
      broadcast("\nERROR: Unable to open file '"+filename+"'\n");
      return -1;
    }
    else
    {
      broadcast("\nINFO: Logging data to '"+filename+"'\n");
    }
  }
  else // Without Dataloger
  {
    broadcast("\nWARNING: Starting test without datalogger.\n");
    broadcast("WARNING: Make sure to save data after running.\n");
  }

  return 0;
}

// Logs Data to Logfile and Streams for Duration
int recordData(long duration, File &log)
{
  // Check INA260
  if(!INA260_ENABLED)
  {
    broadcast("\nERROR: Unable to find INA260.\n");
    return -1;
  }

  broadcast("\nINFO: Logging data for '"+String(duration)+"' seconds.\n");
  broadcast("\nINFO: Enter a number to stop recording data early.\n\n");

  // Log Start Time
  String stamp = "START: ";
  stamp.concat(getTimestamp());

  // Print Timestamp
  logline(log, stamp);

  // Loop Time Variables
  long start = getTime();
  long last = start;
  long now = start;

  // Data Variables
  float current, voltage, power, battery;
  String csv = "TIME,CURRENT,VOLTAGE,POWER,BATTERY";

  // Print Data Labels
  logline(log, csv);

  // Polling Time Loop
  while(now - start < duration)
  {
    //  Check for User Input
    if(getInt() > 0)
    {
      logline("INFO: Stop command receivied.");
      break;
    }

    // Check Time to Log Data
    if(now - last > LOG_INTERVAL)
    {
      // Get Data
      current = ina260.readCurrent();
      voltage = ina260.readBusVoltage();
      power   = ina260.readPower();
      battery = getBatteryVoltage();

      // Arrange in CSV Format
      csv  = String(now)+",";
      csv += String(current)+",";
      csv += String(voltage)+",";
      csv += String(power)+",";
      csv += String(battery);

      // Log Data
      logline(csv);

      // Update Last Log Time
      last = getTime();
    }
    // Poll Current Time
    now = getTime();
  }

  // Log End Time
  stamp = "END: ";
  stamp.concat(getTimestamp());
  logline(log, stamp);

  // Return Success
  return 0;
}

// Attempts to Close Logfile
int closeLogfile(File &log)
{
  if(DATALOGGER_ENABLED)
  {
    log.close();

    if(log)
    {
      return -1;
    }
  }

  return 0;
}

// Logs Message to Logfile & Streams
int logcast(File log, String msg)
{
  if(DATALOGGER_ENABLED)
  {
    log.print(msg);
  }

  broadcast(msg);

  return 0;
}

// Logs Message+\n to Logfile & Streams
int logline(File log, String msg)
{
  if(DATALOGGER_ENABLED)
  {
    log.println(msg);
  }

  broadline(msg);

  return 0;
}
