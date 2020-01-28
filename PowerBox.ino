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
int initModules();
int toggleLED();
int broadcast(String msg);
int commandHandler();
int splitCommand(String command, String argument);
int echoCommand(String command, String argument);
int recordData(long duration);
int printFile(String filename);
int listFiles();

void setup()
{
  // Init Project Objects
  initModules();

  // Opening Message
  broadcast("Initialization Complete!\n");
}

void loop()
{
  // Continuously Read and Process Commands
  commandHandler();
}

int commandHandler()
{
  // Monitor Battery
  if(getBatteryVoltage() < VBATLOW);
  {
    broadcast("ERROR: Battery Low.\n");
    toggleLED();
  }

  // Command Line Variables
  String cmd = "NONE";
  String arg = "NONE";

  // Read In New Command From Stream(s)
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
      ble.readline();
      if(strcmp(ble.buffer, "OK") != 0)
      {
        cmd = ble.buffer;
      }
    }
  }

  // Check If Command Read
  if(cmd != "NONE")
  {
    // Split Cmd & Arg
    splitCommand(cmd, arg);

    /*
      Supported Commands:
        RUN [DURATION] - logs data for duration in minutes
        READ [FILENAME] - prints file to stream(s) available
        LIST - prints list of files on SD to stream(s) available
        RESET - reinitializes all modules of the project
        HELP - prints list of supported commands to stream(s) available
    */

    // RUN [DURATION]
    if(cmd == "RUN" && arg != "NONE")
    {
      // Get Duration
      long duration = arg.toInt();

      // Check Valid Number
      if(duration > 0)
      {
        // Echo Command
        broadcast("OK: ");
        echoCommand(cmd, arg);
        broadcast("NOTE: Enter 'STOP' to quit logging data early.\n");

        // Check Return Value
        if(recordData(duration))
        {
          broadcast("ERROR: Problem occured while logging data.\n");
        }
      }
      else
      {
        // Invalid Duration
        broadcast("ERROR: ");
        echoCommand(cmd, arg);
      }
    }

    // READ [FILENAME]
    else if(cmd == "READ" && arg != "NONE")
    {
      // Check Valid Filename
      if(SD.exists(arg))
      {
        // Echo Command
        broadcast("OK: ");
        echoCommand(cmd, arg);

        // Check Return Value
        if(printFile(arg))
        {
          broadcast("ERROR: Problem occured while reading files.\n");
        }
      }
      else
      {
        // Invalid Filename
        broadcast("ERROR: ");
        echoCommand(cmd ,arg);
      }
    }

    // LIST
    else if(cmd == "LIST" && arg == "NONE")
    {
      // Echo Command
      broadcast("OK: ");
      echoCommand(cmd, arg);

      // Check Return Value
      if(listFiles())
      {
        broadcast("ERROR: Problem occured while listing files.\n");
      }
    }

    // RESET
    else if(cmd == "RESET" && arg == "NONE")
    {
      // Echo Command
      broadcast("OK: ");
      echoCommand(cmd, arg);

      initModules();
    }

    // HELP
    else if(cmd == "HELP" && arg == "NONE")
    {
      // Echo Command
      broadcast("OK: ");
      echoCommand(cmd, arg);

      // List Supported Commands
      broadcast("Commands:\n");
      broadcast("  RUN [duration]\n");
      broadcast("  READ [filename]\n");
      broadcast("  LIST\n");
      broadcast("  RESET\n");
      broadcast("  HELP\n");
    }

    // COMMAND NOT RECOGNIZED
    else
    {
      // Echo Command
      broadcast("ERROR: ");
      echoCommand(cmd, arg);
      broadcast("NOTE: Enter 'HELP' for list of supported commands.\n");
    }
  }

  // Return Success
  return 0;
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

int echoCommand(String command, String argument)
{
  broadcast(command);
  if(argument != "NONE")
  {
    broadcast(" ");
    broadcast(argument);
  }
  broadcast("\n");

  // Return Success
  return 0;
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
    broadcast("ERROR: Unable to init Bluetooth.\n");
    toggleLED();
  }

  if(initDatalogger())
  {
    broadcast("ERROR: Unable to init Datalogger.\n");
    toggleLED();
  }

  if(initINA260())
  {
    broadcast("ERROR: Unable to init INA260.\n");
    toggleLED();
  }

  if(initRTC())
  {
    broadcast("ERROR: Unable to init RTC.\n");
    toggleLED();
  }

  // Return Success
  return 0;
}

int splitCommand(String command, String argument)
{
  // Find if Space Exists
  int index = command.indexOf(' ');

  // Split Around Space
  if(index != -1)
  {
    argument = command.substring(index+1);
    command = command.substring(0, index-1);
  }
  else
  {
    argument = "NONE";
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
      log.println("ERROR: Battery Low");
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
