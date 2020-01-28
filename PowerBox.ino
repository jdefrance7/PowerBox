// Arduino Library
#include <Arduino.h>

// Project Headers
#include "Battery.h"
#include "Bluetooth.h"
#include "Datalogger.h"
#include "INA260.h"
#include "RTC.h"
#include "Serial.h"

// Built-In LED Pin for Adafruit M0
#define LED_PIN 13

// Number of Seconds Between Data Logs
#define LOG_INTERVAL 5

// Function Definitions
int toggleLED();
int broadcast(String msg);
int initError(String msg);
int invalidCommand(String command);
int invalidArgument(String argument);
int initModules();
int splitCommand();
int recordData(long duration);
int printFile(String filename);
int listFiles();

// Command Line Variables
String cmd;
String arg;

void setup()
{
  // Init Library Objects
  initModules();

  // Setup Command Line
  broadcast("> ");
}

void loop()
{
  // Monitor Battery
  if(getBatteryVoltage() < VBATLOW);
  {
    broadcast("ERROR: Battery Low");
    toggleLED();
  }

  // Clear Command & Arg
  cmd = "NONE";
  arg = "NONE";

  // Read New Command
  if(Serial)
  {
    if(Serial.available())
    {
      cmd = Serial.readString();
    }
  }
  else if(ble.isConnected())
  {
    if(ble.available())
    {
      cmd = ble.readline();
    }
  }

  // Check For Input
  if(cmd != "NONE")
  {
    // Split Cmd & Arg
    splitCommand();

    // Command Cases
    if(cmd == "RUN")
    {
      if(arg == "NONE")
      {
        invalidCommand(cmd);
      }
      else
      {
        long duration = arg.toInt();
        if(duration > 0)
        {
          broadcast("OK.\n");
          if(recordData(duration))
          {
            broadcast("ERROR: Problem occured while recording data.\n");
          }
        }
        else
        {
          invalidArgument(arg);
        }
      }
    }
    else if(cmd == "READ")
    {
      if(arg == "NONE")
      {
        invalidCommand(cmd);
      }
      else
      {
        if(SD.exists(arg))
        {
          broadcast("OK.\n");
          if(printFile(arg))
          {
            broadcast("ERROR: Problem occured while reading files.\n");
          }
        }
        else
        {
          invalidArgument(arg);
        }
      }
    }
    else if(cmd == "LIST")
    {
      broadcast("OK\n");
      if(listFiles())
      {
        broadcast("ERROR: Problem occured while listing files.\n");
      }
    }
    else if(cmd == "RESET")
    {
      broadcast("OK.\n");
      initModules();
    }
    else
    {
      broadcast("ERROR: ");
      broadcast(cmd);
      if(arg != "NONE")
      {
        broadcast(" ");
        broadcast(arg);
      }
      broadcast("\n");
    }
    broadcast("> ");
  }
}

int toggleLED()
{
  while(1)
  {
    digitalWrite(LED_PIN, HIGH);
    delay(300);
    digitalWrite(LED_PIN, LOW);
    delay(300);
  }
}

int broadcast(String msg)
{
  if(Serial)
  {
    Serial.print(msg);
  }
  if(ble.isConnected())
  {
    ble.print(msg);
  }
}

int initError(String msg)
{
  broadcast("ERROR: Unable to init '");
  broadcast(msg);
  broadcast("'\n");
}

int invalidCommand(String command)
{
  broadcast("ERROR: Invalid command '");
  broadcast(command);
  broadcast("'\n");
}

int invalidArgument(String argument)
{
  broadcast("ERROR: Invalid argument '");
  broadcast(argument);
  broadcast("'\n");
}

int initModules()
{
  pinMode(LED_PIN, OUTPUT);

  if(initSerial())
  {
    toggleLED();
  }

  if(initBluetooth())
  {
    initError("Bluetooth");
    toggleLED();
  }

  if(initDatalogger())
  {
    initError("Datalogger");
    toggleLED();
  }

  if(initINA260())
  {
    initError("INA260");
    toggleLED();
  }

  if(initRTC())
  {
    initError("RTC");
    toggleLED();
  }
}

int splitCommand()
{
  // Find if Space Exists
  int index = cmd.indexOf(' ');

  // Split Around Space
  if(index != -1)
  {
    arg = cmd.substring(index+1);
    cmd = cmd.substring(0, index-1);
  }
  else
  {
    arg = "NONE";
  }

  // Return Success
  return 0;
}

int recordData(long duration)
{
  // Datalogger Variables
  File log;
  String filename;
  char charName[16];

  // Find Next Available Filename
  for(int n = 0; n < 1000; n++)
  {
    filename = "/log";
    filename.concat(n);
    filename.concat(".csv");

    // Convert String to Char Array
    filename.toCharArray(charName, 16);

    if(!SD.exists(charName));
    {
      break;
    }
  }

  // Open File
  if(openFile(log, charName, FILE_WRITE))
  {
    // Return Error
    broadcast("ERROR: Unable to open file '");
    broadcast(filename);
    broadcast("'\n");
    return -1;
  }

  // Log Start Time
  DateTime start = rtc.now();
  log.println(start.timestamp(DateTime::TIMESTAMP_FULL));

  // Loop Time variables
  DateTime last = start;
  DateTime now = start;

  // User Input Variable
  String input;

  // Polling Loop
  while(now.unixtime() - start.unixtime() < duration)
  {
    // Monitor Battery
    if(getBatteryVoltage() < VBATLOW);
    {
      broadcast("ERROR: Battery Low");
      break;
    }

    // Check for User Input
    if(Serial)
    {
      if(Serial.available())
      {
        input = Serial.readString();
      }
    }
    if(ble.isConnected())
    {
      if(ble.available())
      {
        input = ble.readline();
      }
    }
    if(input == "STOP")
    {
      break;
    }

    // Check to Log Data
    if(now.unixtime() - last.unixtime() > LOG_INTERVAL)
    {
      // Get Data
      float current = ina260.readCurrent();
      float voltage = ina260.readBusVoltage();
      float power   = ina260.readPower();

      // Log Data
      log.print(now.unixtime());
      log.print(',');
      log.print(current);
      log.print(',');
      log.print(voltage);
      log.print(',');
      log.print(power);
      log.print('\n');

      // Print Data
      if(Serial)
      {
        Serial.print(now.unixtime());
        Serial.print(',');
        Serial.print(current);
        Serial.print(',');
        Serial.print(voltage);
        Serial.print(',');
        Serial.print(power);
        Serial.print('\n');
      }
      if(ble.isConnected())
      {
        ble.print(now.unixtime());
        ble.print(',');
        ble.print(current);
        ble.print(',');
        ble.print(voltage);
        ble.print(',');
        ble.print(power);
        ble.print('\n');
      }

      // Update Last Log Time
      last = rtc.now();
    }
    // Poll Current Time
    now = rtc.now();
  }

  // Log End Time
  DateTime end = rtc.now();
  log.println(end.timestamp(DateTime::TIMESTAMP_FULL));

  // Close File
  closeFile(log);

  // Return Success
  return 0;
}

int printFile(String filename)
{
  // Line Variables
  char character;
  String line = "";

  // Convert Filename to Char Array
  char charName[16];
  filename.toCharArray(charName, 16);

  // Open File
  File logfile;
  if(openFile(logfile, charName, FILE_READ))
  {
    // Return Error
    broadcast("ERROR: Unable to open file '");
    broadcast(filename);
    broadcast("'\n");
    return -1;
  }

  // Read File Line by Line
  while(logfile.available())
  {
    // Read Next Char
    character = logfile.read();

    if(character == '\n')
    {
      // Print Line
      if(Serial)
      {
        Serial.println(line);
      }
      if(ble.isConnected())
      {
        ble.println(line);
      }

      // Clear Line
      line = "";
    }
    else
    {
      // Add Character to Line
      line.concat(character);
    }
  }

  // Return Success
  return 0;
}

int listFiles()
{
  // Base Directory
  File root = SD.open("/");

  if(!root)
  {
    // Return Error
    broadcast("ERROR: Unable to open root.");
    return -1;
  }

  // Iterate Through Base Directory
  File entry;
  while(1)
  {
    entry = root.openNextFile();
    if(!entry)
    {
      break;
    }
    else if(entry.isDirectory())
    {
      continue;
    }
    else
    {
      if(Serial)
      {
        Serial.println(entry.name());
      }
      if(ble.isConnected())
      {
        ble.println(entry.name());
      }
    }
  }

  // Close Root
  root.close();

  // Return Success
  return 0;
}
