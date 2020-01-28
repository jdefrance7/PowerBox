// Arduino Library
#include <Arduino.h>

// Project Headers
#include "Battery.h"
#include "Bluetooth.h"
#include "Datalogger.h"
#include "INA260.h"
#include "RTC.h"
#include "Serial.h"

// Debug Flag
#define DEBUG

// Built-In LED Pin for Adafruit M0
#define LED_PIN 13

// Number of Seconds Between Data Logs
#define LOG_INTERVAL 5

// Function Declarations
int initModules();
int toggleLED();
int broadcast(String msg);
int commandHandler();
String getCommand();
int splitCommand(String command, String argument);
int echoCommand(String command, String argument);
long getTime();
String getTimestamp();
int recordData(long duration);
int printFile(String filename);
int listFiles();

// Module Statuses
bool SERIAL_ENABLED = true;
bool BLUETOOTH_ENABLED = true;
bool DATALOGGER_ENABLED = true;
bool RTC_ENABLED = true;
bool INA260_ENABLED = true;

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

  // Poll Stream(s) for Command
  String cmd = getCommand();

  #ifdef DEBUG
  broadcast("CMD: ");
  broadcast(cmd);
  broadcast("\n");
  #endif

  // Check if Command was Read
  if(cmd != "NONE")
  {
    // String for Arg
    String arg = "NONE";

    // Split Cmd & Arg
    splitCommand(cmd, arg);

    /*
      Supported Commands:
        RUN [DURATION] - logs data for duration in minutes
        READ [FILENAME] - prints file to stream(s) available
        LIST - prints list of files on SD to stream(s) available
        RESET - reinitializes all modules of the project
        STATUS - prints status of modules to stream(s) available
        HELP - prints list of supported commands to stream(s) available
    */

    // COMMAND: RUN [DURATION]
    if(cmd == "RUN" && arg != "NONE")
    {
      // Get Duration
      long duration = arg.toInt();

      #ifdef DEBUG
      broadcast("DURATION: ");
      broadcast(String(duration));
      broadcast("\n");
      #endif

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

    // COMMAND: READ [FILENAME]
    else if(cmd == "READ" && arg != "NONE")
    {

      #ifdef DEBUG
      broadcast("FILENAME: ");
      broadcast(arg);
      broadcast("\n");
      #endif

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

      // Reset Modules
      initModules();
    }

    // COMMAND: STATUS
    else if(cmd == "STATUS" && arg == "NONE")
    {
      // Echo Command
      broadcast("OK: ");
      echoCommand(cmd, arg);

      // Broadcast Module Statuses
      broadcast("SERIAL: ");
      broadcast(String(SERIAL_ENABLED));
      broadcast("\nBLUETOOTH: ");
      broadcast(String(BLUETOOTH_ENABLED));
      broadcast("\nDATALOGGER: ");
      broadcast(String(DATALOGGER_ENABLED));
      broadcast("\nRTC: ");
      broadcast(String(RTC_ENABLED));
      broadcast("\nINA260: ");
      broadcast(String(INA260_ENABLED));
      broadcast("\n");
    }

    // COMMAND: HELP
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
      broadcast("  STATUS\n");
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
  if(SERIAL_ENABLED)
  {
    Serial.print(msg);
  }
  if(BLUETOOTH_ENABLED)
  {
    if(ble.isConnected())
    {
      ble.print(msg);
    }
  }
}

String getCommand()
{
  // Read In New Command From Stream(s)
  if(SERIAL_ENABLED)
  {
    if(Serial.available())
    {
      return Serial.readString();
    }
  }
  else if(BLUETOOTH_ENABLED)
  {
    if(ble.isConnected())
    {
      if(ble.available())
      {
        ble.readline();
        if(strcmp(ble.buffer, "OK") != 0)
        {
          return String(ble.buffer);
        }
      }
    }
  }
  return String("NONE");
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
  // Set Onboard LED Pin to Output
  pinMode(LED_PIN, OUTPUT);

  // Attempt to Init Serial Module
  if(initSerial())
  {
    SERIAL_ENABLED = false;
  }

  // Attempt to Init Bluetooth Module
  if(initBluetooth())
  {
    BLUETOOTH_ENABLED = false;
  }

  // Critical Error if Both Streams are Down
  if(!SERIAL_ENABLED && !BLUETOOTH_ENABLED)
  {
    toggleLED();
  }

  // If Serial is Down, Use Bluetooth
  else if(!SERIAL_ENABLED)
  {
    broadcast("ERROR: Unable to init Serial.\n");
  }

  // If Bluetooth is down, Use Serial
  else if(!BLUETOOTH_ENABLED)
  {
    broadcast("ERROR: Unable to init Bluetooth.\n");
  }

  // Attempt to Init Datalogger
  if(initDatalogger())
  {
    DATALOGGER_ENABLED = false;
    broadcast("ERROR: Unable to init Datalogger.\n");
  }

  // Attempt to Init INA260 (Critical Component)
  if(initINA260())
  {
    broadcast("ERROR: Unable to init INA260.\n");
    toggleLED();
  }

  // Attempt to Init RTC
  if(initRTC())
  {
    RTC_ENABLED = false;
    broadcast("ERROR: Unable to init RTC.\n");
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

long getTime()
{
  if(RTC_ENABLED)
  {
    return ((rtc.now()).unixtime());
  }
  return (millis()/1000);
}

String getTimestamp()
{
  if(RTC_ENABLED)
  {
    return rtc.now().timestamp(DateTime::TIMESTAMP_FULL);
  }
  String seconds = String(millis()/1000);
  seconds.concat("s");
  return seconds;
}

int recordData(long duration)
{
  // Check INA260
  if(!INA260_ENABLED)
  {
    broadcast("ERROR: Unable to find INA260.\n");
    return -1;
  }

  // Create Log File
  File log;

  if(DATALOGGER_ENABLED)
  {
    // Filename Variables
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
  }
  else // Without Dataloger
  {
    broadcast("WARNING: Starting test without datalogger.\n");
    broadcast("WARNING: Make sure to save data after running.\n");
  }

  // Log Start Time
  String stamp = "START: ";
  stamp.concat(getTimestamp());

  if(DATALOGGER_ENABLED)
  {
    log.print(stamp);
  }
  if(SERIAL_ENABLED)
  {
    Serial.print(stamp);
  }
  if(BLUETOOTH_ENABLED)
  {
    if(ble.isConnected())
    {
      ble.println(stamp);
    }
  }

  // Loop Time Variables
  long start = getTime();
  long last = start;
  long now = start;

  // Data Variables
  float current, voltage, power;

  // Polling Time Loop
  while(now - start < duration)
  {
    // Monitor Battery
    if(getBatteryVoltage() < VBATLOW);
    {
      if(DATALOGGER_ENABLED)
      {
        log.println("ERROR: Battery Low.\n");
      }
      broadcast("ERROR: Battery Low.\n");

      // Break from Loop & Close File
      break;
    }

    //  Check for User Input
    if(getCommand() == "STOP")
    {
      // Break from Loop & Close File
      break;
    }

    // Check Time to Log Data
    if(now - last > LOG_INTERVAL)
    {
      // Get Data
      current = ina260.readCurrent();
      voltage = ina260.readBusVoltage();
      power   = ina260.readPower();

      // Log Data
      if(DATALOGGER_ENABLED)
      {
        log.print(now);
        log.print(',');
        log.print(current);
        log.print(',');
        log.print(voltage);
        log.print(',');
        log.print(power);
        log.print('\n');
      }

      // Print Data to Serial
      if(SERIAL_ENABLED)
      {
        Serial.print(now);
        Serial.print(',');
        Serial.print(current);
        Serial.print(',');
        Serial.print(voltage);
        Serial.print(',');
        Serial.print(power);
        Serial.print('\n');
      }

      // Print Data to Bluetooth
      if(BLUETOOTH_ENABLED)
      {
        if(ble.isConnected())
        {
          ble.print(now);
          ble.print(',');
          ble.print(current);
          ble.print(',');
          ble.print(voltage);
          ble.print(',');
          ble.print(power);
          ble.print('\n');
        }
      }

      // Update Last Log Time
      last = getTime();
    }
    // Poll Current Time
    now = getTime();
  }

  // Log End Time
  stamp = "END: ";
  stamp.concat(getTimestamp());
  if(DATALOGGER_ENABLED)
  {
    log.println(stamp);
  }
  if(SERIAL_ENABLED)
  {
    Serial.println(stamp);
  }
  if(BLUETOOTH_ENABLED)
  {
    if(ble.isConnected())
    {
      ble.println(stamp);
    }
  }

  // Close File
  closeFile(log);

  // Return Success
  return 0;
}

int printFile(String filename)
{
  // Check Datalogger
  if(!DATALOGGER_ENABLED)
  {
    broadcast("ERROR: Datalogger not available.\n");
    return -1;
  }

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
      if(SERIAL_ENABLED)
      {
        Serial.println(line);
      }
      if(BLUETOOTH_ENABLED)
      {
        if(ble.isConnected())
        {
          ble.println(line);
        }
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
  // Check Datalogger
  if(!DATALOGGER_ENABLED)
  {
    broadcast("ERROR: Datalogger unavailable.\n");
    return -1;
  }

  // Open Base Directory
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
      // Reached End of Root
      break;
    }
    else if(entry.isDirectory())
    {
      // Skip Folders
      continue;
    }
    else
    {
      // Print Filename
      if(SERIAL_ENABLED)
      {
        Serial.println(entry.name());
      }
      if(BLUETOOTH_ENABLED)
      {
        if(ble.isConnected())
        {
          ble.println(entry.name());
        }
      }
    }
  }

  // Close Root
  root.close();

  // Return Success
  return 0;
}
