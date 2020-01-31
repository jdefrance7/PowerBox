// Arduino Library
#include <Arduino.h>

// Board: Adafruit Feather M0
// Programmer: AVRISP mkII

// Build Option: Debug Information
#define DEBUG

// Build Option: OLED Display
#define OLED

// Project Headers
#include "Battery.h"
#include "Bluetooth.h"
#include "Datalogger.h"
#include "INA260.h"
#include "RTC.h"
#include "Serial.h"

#ifdef OLED
#include "OLED.h"
#endif

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
bool OLED_ENABLED = true;

void setup()
{
  // Init Project Objects
  initModules();

  // Opening Message
  broadcast("Initialization Complete!\n");

  #ifdef OLED
  delay(5000);
  oled.clearDisplay();
  oled.println("Initialization");
  oled.println("Complete!");
  oled.display();
  delay(2000);
  #endif
}

void loop()
{
  // Continuously Read and Process Commands
  commandHandler();
}

int commandHandler()
{
  
//  // Monitor Battery
//  float battery = getBatteryVoltage();
//  if(battery < VBATLOW);
//  {
//    broadcast("ERROR: Battery Low.\n");
//
//    #ifdef OLED
//    oled.clearDisplay();
//    oled.print("ERROR: Battery Low");
//    oled.display();
//    delay(2000);
//    #endif
//    
//    toggleLED();
//  }

  // Poll Stream(s) for Command
  String cmd = getCommand();

  #ifdef OLED
  oled.clearDisplay();
  oled.print("CMD: ");
  oled.print(cmd);
  oled.display();
  delay(3000);
  #endif

  // Check if Command was Read
  if(cmd != "NONE")
  {
    #ifdef DEBUG
    broadcast("RAW: ");
    broadcast(cmd);
    broadcast("\n");
    #endif

    // String for Arg
    String arg = "NONE";

    // Split Cmd & Arg
    splitCommand(cmd, arg);

    #ifdef DEBUG
    broadcast("CMD: ");
    broadcast(cmd);
    broadcast("\n");
    broadcast("ARG: ");
    broadcast(arg);
    broadcast("\n");
    #endif

    #ifdef OLED
    oled.clearDisplay();
    oled.print("CMD: ");
    oled.println(cmd);
    oled.print("ARG: ");
    oled.println(arg);
    oled.print("RSP: ");
    oled.display();
    #endif

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
        broadcast("INFO: Enter 'STOP' to quit logging data early.\n");

        #ifdef OLED
        oled.println("OK");
        oled.print("MSG: ");
        oled.println("Enter 'STOP' to quit early.");
        oled.display();
        #endif

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

        #ifdef OLED
        oled.println("ERROR");
        oled.print("MSG: ");
        oled.println("Invalid duration.");
        oled.display();
        #endif
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

        #ifdef OLED
        oled.println("OK");
        oled.print("MSG: ");
        oled.println("NONE");
        oled.display();
        #endif

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

        #ifdef OLED
        oled.println("ERROR");
        oled.print("MSG: ");
        oled.println("Invalid filename.");
        oled.display();
        #endif
      }
    }

    // LIST
    else if(cmd == "LIST" && arg == "NONE")
    {
      // Echo Command
      broadcast("OK: ");
      echoCommand(cmd, arg);

      #ifdef OLED
      oled.println("OK");
      oled.print("MSG: ");
      oled.println("NONE");
      oled.display();
      #endif

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

      #ifdef OLED
      oled.println("OK");
      oled.print("MSG: ");
      oled.println("NONE");
      oled.display();
      #endif

      // Reset Modules
      initModules();
    }

    // COMMAND: STATUS
    else if(cmd == "STATUS" && arg == "NONE")
    {
      // Echo Command
      broadcast("OK: ");
      echoCommand(cmd, arg);

      #ifdef OLED
      oled.println("OK");
      oled.print("MSG: ");
      oled.println("NONE");
      oled.display();
      #endif

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

      #ifdef OLED
      oled.clearDisplay();
      oled.print("SERIAL: ");
      oled.println(String(SERIAL_ENABLED));
      oled.print("BLUETOOTH: ");
      oled.println(String(BLUETOOTH_ENABLED));
      oled.print("DATALOGGER: ");
      oled.println(String(DATALOGGER_ENABLED));
      oled.print("RTC: ");
      oled.print(String(RTC_ENABLED));
      oled.print("   ");
      oled.print("INA260: ");
      oled.println(String(INA260_ENABLED));
      oled.display();
      #endif
    }

    // COMMAND: HELP
    else if(cmd == "HELP" && arg == "NONE")
    {
      // Echo Command
      broadcast("OK: ");
      echoCommand(cmd, arg);

      #ifdef OLED
      oled.println("OK");
      oled.print("MSG: ");
      oled.println("NONE");
      oled.display();
      #endif

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
      broadcast("INFO: Enter 'HELP' for list of supported commands.\n");

      #ifdef OLED
      oled.println("ERROR");
      oled.print("MSG: ");
      oled.println("Invalid command.");
      oled.display();
      delay(1000);
      #endif
    }
  }

  // Return Success
  return 0;
}

int toggleLED()
{

  #ifdef OLED
  oled.clearDisplay();
  oled.print("FATAL ERROR");
  oled.display();
  #endif

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
  if(BLUETOOTH_ENABLED)
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

  // Attempt to Init OLED if Included
  #ifdef OLED
  if(initOLED())
  {
    OLED_ENABLED = false;
  }
  oled.clearDisplay();
  oled.print("SERIAL: ");
  oled.display();
  #endif

  // Attempt to Init Serial Module
  if(initSerial())
  {
    SERIAL_ENABLED = false;
  }

  #ifdef OLED
  if(SERIAL_ENABLED)
  {
    oled.println("YES"); 
  }
  else
  {
    oled.println("NO");
  }
  oled.display();
  #endif

  #ifdef OLED
  oled.print("BLUETOOTH: ");
  oled.display();
  #endif

  // Attempt to Init Bluetooth Module
  if(initBluetooth())
  {
    BLUETOOTH_ENABLED = false;
  }

  #ifdef OLED
  if(BLUETOOTH_ENABLED)
  {
    oled.println("YES");
  }
  else
  {
    oled.println("NO");
  }
  oled.display();
  #endif

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

  #ifdef OLED
  oled.print("DATALOGGER: ");
  oled.display();
  #endif

  // Attempt to Init Datalogger
  if(initDatalogger())
  {
    DATALOGGER_ENABLED = false;
    broadcast("ERROR: Unable to init Datalogger.\n");
  }

  #ifdef OLED
  if(DATALOGGER_ENABLED)
  {
    oled.println("YES");
  }
  else
  {
    oled.println("NO");
  }
  #endif

  #ifdef OLED
  oled.print("INA260: ");
  oled.display();
  #endif

  // Attempt to Init INA260 (Critical Component)
  if(initINA260())
  {
    INA260_ENABLED = false;
    broadcast("ERROR: Unable to init INA260.\n");
    // toggleLED();
  }

  #ifdef OLED
  if(INA260_ENABLED)
  {
    oled.print("YES");
  }
  else
  {
    oled.print("NO");
  }
  #endif

  #ifdef OLED
  oled.print(" RTC: ");
  oled.display();
  #endif

  // Attempt to Init RTC
  if(initRTC())
  {
    RTC_ENABLED = false;
    broadcast("ERROR: Unable to init RTC.\n");
  }

  #ifdef OLED
  if(RTC_ENABLED)
  {
    oled.println("YES");
  }
  else
  {
    oled.println("NO");
  }
  #endif

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

    #ifdef OLED
    oled.clearDisplay();
    oled.print("ERROR: Unable to find INA260.\n");
    oled.display();
    delay(1000);
    #endif

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

      #ifdef OLED
      oled.clearDisplay();
      oled.println("ERROR: Unable to open file.");
      oled.print("FILE: ");
      oled.println(filename);
      oled.display();
      delay(1000);
      #endif

      return -1;
    }
  }
  else // Without Dataloger
  {
    broadcast("WARNING: Starting test without datalogger.\n");
    broadcast("WARNING: Make sure to save data after running.\n");

    #ifdef OLED
    oled.clearDisplay();
    oled.println("WARNING: Starting test.");
    oled.println("without datalogger.");
    oled.println("WARNING: Make sure to save");
    oled.println("data after running.");
    oled.display();
    delay(1000);
    #endif
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

  #ifdef OLED
  oled.clearDisplay();
  oled.print("START: ");
  oled.println(stamp);
  oled.display();
  delay(1000);
  #endif

  // Loop Time Variables
  long start = getTime();
  long last = start;
  long now = start;

  // Data Variables
  float current, voltage, power, battery;

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

      #ifdef OLED
      oled.clearDisplay();
      oled.print("ERROR: Battery Low");
      oled.display();
      delay(1000);
      #endif

      // Break from Loop & Close File
      break;
    }

    //  Check for User Input
    if(getCommand() == "STOP")
    {
      broadcast("INFO: 'STOP' command received.\n");

      #ifdef OLED
      oled.clearDisplay();
      oled.print("STOP command received.");
      oled.display();
      delay(1000);
      #endif

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
      battery = getBatteryVoltage();


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
        log.print(',');
        log.print(battery);
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
        Serial.print(',');
        Serial.print(battery);
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
          ble.print(',');
          ble.print(battery);
          ble.print('\n');
        }
      }

      #ifdef OLED
      oled.clearDisplay();
      oled.print("CURRENT: ");
      oled.print(current);
      oled.println(" mA");
      oled.print("VOLTAGE: ");
      oled.print(voltage);
      oled.println(" V");
      oled.print("POWER: ");
      oled.print(power);
      oled.println(" mW");
      oled.print("BATTERY: ");
      oled.print(battery);
      oled.println(" V");
      oled.display();
      #endif

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

  #ifdef OLED
  oled.clearDisplay();
  oled.print("END: ");
  oled.print(stamp);
  oled.display();
  delay(1000);
  #endif

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

    #ifdef OLED
    oled.clearDisplay();
    oled.print("ERROR: Dataloger not available.\n");
    oled.display();
    delay(1000);
    #endif

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

    #ifdef OLED
    oled.clearDisplay();
    oled.println("ERROR: Unable to open file.");
    oled.print("FILE: ");
    oled.println(filename);
    oled.display();
    delay(1000);
    #endif

    return -1;
  }

  #ifdef OLED
  oled.clearDisplay();
  oled.print("Reading file...");
  oled.display();
  #endif

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

  #ifdef OLED
  oled.print("Read Complete!");
  oled.display();
  delay(1000);
  #endif

  // Return Success
  return 0;
}

int listFiles()
{
  // Check Datalogger
  if(!DATALOGGER_ENABLED)
  {
    broadcast("ERROR: Datalogger unavailable.\n");

    #ifdef OLED
    oled.clearDisplay();
    oled.print("ERROR: Datalogger unavailable.\n");
    oled.display();
    delay(1000);
    #endif

    return -1;
  }

  // Open Base Directory
  File root = SD.open("/");
  if(!root)
  {
    // Return Error
    broadcast("ERROR: Unable to open root.");

    #ifdef OLED
    oled.clearDisplay();
    oled.print("ERROR: Unable to open root.");
    oled.display();
    delay(1000);
    #endif

    return -1;
  }

  #ifdef OLED
  oled.clearDisplay();
  oled.print("Listing files...");
  oled.display();
  #endif

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

  #ifdef OLED
  oled.clearDisplay();
  oled.print("List Complete!");
  oled.display();
  delay(1000);
  #endif

  // Return Success
  return 0;
}
