// PROGRAM NOTES
// Board: Adafruit Feather M0
// Programmer: AVRISP mkII

// Build Option: Debug Information
#define DEBUG

// Build Option: OLED Display
#define OLED

// Arduino Library
#include <Arduino.h>

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

int getInput(int input);
int getDuration(long duration);
int getFilename(char* filename);

int recordData(long duration);
long getTime();
String getTimestamp();

int printFile(String filename);

int printStatus();

int printInfo();

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
}

void loop()
{
  // Continuously Read and Process Commands
  commandHandler();
}

int commandHandler()
{
  // User Input Variable
  static int input;

  // Print Main Menu Options
  broadcast("MAIN MENU: \n");
  broadcast(" 1. RECORD DATA \n");
  broadcast(" 2. READ FILE \n");
  broadcast(" 3. MODULE STATUS \n");
  broadcast(" 4. RESET DEVICE \n");
  broadcast(" 5. INFORMATION \n");

  while(1)
  {
    // Get User Input
    if(!getInput(input))
    {
      // No Input
      if(input == 0)
      {
        continue;
      }

      // Record Data
      else if(input == 1)
      {
        long duration;
        if(getDuration())
        {

        }
        else if(recordData(duration))
        {

        }
        else
        {

        }
      }

      // Read File
      else if(input == 2)
      {
        char* filename;
        if(getFilename(filename))
        {

        }
        else if(readFile(filename))
        {

        }
        else
        {

        }
      }

      // Module Status
      else if(input == 3)
      {
        if(printStatus())
        {

        }
      }

      // Reset Device
      else if(input == 4)
      {
        if(initModules())
        {

        }
      }

      // Information
      else if(input == 5)
      {
        if(printInfo())
        {

        }
      }

      // Invalid Entry
      else
      {
        broadcast("ERROR: Invalid entry '"+String(input)+"'.\n");
      }
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

int getInput(int input)
{
  input = 0;

  if(SERIAL_ENABLED)
  {
    if(Serial.available())
    {
      input = int(Serial.parseInt());
    }
  }

  if(BLUETOOTH_ENABLED)
  {
    if(ble.isConnected())
    {
      if(ble.available())
      {
        input = int(ble.readline_parseInt());
      }
    }
  }

  // Invalid Input
  if(input < 0)
  {
    broadcast("ERROR: Invalid input '"+String(input)+"'.\n");
    return -1;
  }

  // No Input
  else if(input == 0)
  {
    return -1;
  }

  // Valid Input
  return 0;
}

int getDuration(long duration)
{
  // Check for Errors
  if(!SERIAL_ENABLED && !BLUETOOTH_ENABLED)
  {
    broadcast("ERROR: No streams available.\n");
    duration = -1;
    return -1;
  }

  // Clear Duration
  duration = 0;

  broadcast("Enter Duration in Minutes: \n");

  while(1)
  {
    if(SERIAL_ENABLED)
    {
      if(Serial.available())
      {
        // Returns '-1' on failure.
        duration = long(Serial.parseInt());
      }
    }

    if(BLUETOOTH_ENABLED)
    {
      if(ble.isConnected())
      {
        if(ble.available())
        {
          // Returns '-1' on failure, '0' on no data.
          duration = long(ble.readline_parseInt());
        }
      }
    }

    // No Input
    if(duration == 0)
    {
      continue;
    }

    // Invalid Duration
    else if(duration < 0)
    {
      broadcast("ERROR: Invalid duration '"+String(duration)+"'.\n");
      broadcast("Enter Duration in Minutes: \n");
    }

    // Valid Duration
    else // duration > 0
    {
      break;
    }
  }

  return 0;
}

int getFilename(char* filename)
{
  // Check for Errors
  if(!DATALOGGER_ENABLED)
  {
    broadcast("ERROR: Datalogger not available.\n");
    return -1;
  }

  // Open Base Directory
  File root = SD.open("/");
  if(!root)
  {
    broadcast("ERROR: Unable to open root.");
    return -1;
  }

  // Start with First File
  int filenumber = 1;

  // Iterate through Root Directory
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
      broadcast(String(filenumber++)+". "+entry.name()+"\n");
    }
  }

  // Ask User for Filenumber
  broadcast("Enter Filenumber: ");

  int number = 0;

  while(1)
  {
    if(SERIAL_ENABLED)
    {
      if(Serial.available())
      {
        number = Serial.parseInt();
      }
    }
    if(BLUETOOTH_ENABLED)
    {
      if(ble.isConnected())
      {
        if(ble.available())
        {
          number = ble.readline_parseInt();
        }
      }
    }

    // No Entry
    if(number == 0)
    {
      continue;
    }

    // Entry out of Range
    else if(number <= 0 || number >= filenumber)
    {
      broadcast("ERROR: Invalid filenumber '"+String(number)+"'.\n");
    }

    // Entry in Range
    else
    {
      break;
    }
  }

  // Re-Iterate Through Root Directory
  root.rewindDirectory();
  filenumber = 1;

  while(1)
  {
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
      if(number == filenumber)
      {
        filename = entry.name();
        entry.close();
        root.close();
        return 0;
      }

      // Try Next File
      else
      {
        filenumber++;
      }
    }
  }

  // Close Root Directory
  root.close();

  // File Not Found
  return -1;
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
  return String(millis()/1000)+"s";
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
    INA260_ENABLED = false;
    broadcast("ERROR: Unable to init INA260.\n");
    // toggleLED();
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

      // Break from Loop & Close File
      break;
    }

    //  Check for User Input
    if(getCommand() == "STOP")
    {
      broadcast("INFO: 'STOP' command received.\n");

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

int printFile(char* filename)
{
  // Check Datalogger
  if(!DATALOGGER_ENABLED)
  {
    broadcast("ERROR: Datalogger not available.\n");

    return -1;
  }

  // Open File
  File logfile;
  if(openFile(logfile, filename, FILE_READ))
  {
    broadcast("ERROR: Unable to open file '"+String(filename)+"'.\n");
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

int printStatus()
{
  broadcast("MODULE STATUS: \n");

  broadcast("SERIAL: "+String(SERIAL_ENABLED)+"\n");

  broadcast("BLUETOOTH: "+String(BLUETOOTH_ENABLED)+"\n");

  broadcast("DATALOGGER: "+String(DATALOGGER_ENABLED)+"\n");

  broadcast("RTC: "+String(RTC_ENABLED)+"\n");

  broadcast("INA260: "+String(INA260_ENABLED)+"\n");

  broadcast("BATTERY: "+String(getBatteryVoltage())+" V\n");

  return 0;
}

int printInfo()
{
  broadcast("INFORMATION: Enter numbers to navigate menus. \n");

  broadcast("1. RECORD DATA - Logs data for selected duration.\n");

  broadcast("2. READ FILE - Prints selected file contents to streams.\n");

  broadcast("3. MODULE STATUS - Prints module status to streams.\n");

  broadcast("4. RESET DEVICE - Restarts device, resetting modules.\n");

  broadcast("5. INFORMATION - Prints menu information.\n");

  return 0;
}
