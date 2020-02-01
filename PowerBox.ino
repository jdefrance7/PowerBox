// PROGRAM NOTES
// Board: Adafruit Feather M0
// Programmer: AVRISP mkII

// Build Option: Debug Information
#define DEBUG

// Build Option: OLED Display
//#define OLED

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

int getInt();
long getLong();

int getDuration(long &duration);
int getFilenumber(int &filenumber);

//int getFilename(char* &filename);

int recordData(long duration);
long getTime();
String getTimestamp();

int printFile(int filenumber);
//int printFile(char* filename);

int printStatus();

int printInfo();

// Module Statuses
bool SERIAL_ENABLED = false;
bool BLUETOOTH_ENABLED = false;
bool DATALOGGER_ENABLED = false;
bool RTC_ENABLED = false;
bool INA260_ENABLED = false;
bool OLED_ENABLED = false;

void setup()
{
  // Init Project Objects
  initModules();

  // General Info
  broadcast("\nINFO: Enter numbers to navigate options.\n");
}

void loop()
{
  // Continuously Read and Process Commands
  commandHandler();
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
      ble.waitForOK();
    }
  }
}

int getInt()
{
  // Check for Errors
  if(!SERIAL_ENABLED && !BLUETOOTH_ENABLED)
  {
    broadcast("ERROR: No streams available.\n");
    return -1;
  }

  if(SERIAL_ENABLED)
  {
    if(Serial.available())
    {
      return int(Serial.parseInt());
    }
  }

  if(BLUETOOTH_ENABLED)
  {
    if(ble.isConnected())
    {
      if(ble.available())
      {
        return int(ble.readline_parseInt());
      }
    }
  }

  return 0;
}

long getLong()
{
  // Check for Errors
  if(!SERIAL_ENABLED && !BLUETOOTH_ENABLED)
  {
    broadcast("ERROR: No streams available.\n");
    return -1;
  }

  if(SERIAL_ENABLED)
  {
    if(Serial.available())
    {
      // Returns '-1' on failure.
      return long(Serial.parseInt());
    }
  }

  if(BLUETOOTH_ENABLED)
  {
    if(ble.isConnected())
    {
      if(ble.available())
      {
        // Returns '-1' on failure, '0' on no data.
        return long(ble.readline_parseInt());
      }
    }
  }

  return 0;
}

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
      broadcast("\nINFO: Enter Duration in Seconds: ");
    }
  }
  duration = value;

  broadcast("\nRECEIVED: "+String(duration)+"\n");

  return 0;
}

int getFilenumber(int &filenumber)
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
      broadcast(String(entryNumber++)+". "+entry.name()+"\n");
    }
  }

  // Ask User for Filenumber
  broadcast("\nINFO: Enter Filenumber: ");

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
    else if(number <= 0 || number >= entryNumber)
    {
      broadcast("ERROR: Invalid filenumber '"+String(number)+"'.\n");
    }

    // Entry in Range
    else
    {
      break;
    }
  }

  broadcast("\nFILENUMBER: "+String(number));

  filenumber = number;

  // Close Root
  root.close();

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
  else
  {
    SERIAL_ENABLED = true;
  }

  // Attempt to Init Bluetooth Module
  if(initBluetooth())
  {
    BLUETOOTH_ENABLED = false;
  }
  else
  {
    BLUETOOTH_ENABLED = true;
  }

  // Critical Error if Both Streams are Down
  if(!SERIAL_ENABLED && !BLUETOOTH_ENABLED)
  {
    toggleLED();
  }
  else
  {
    broadcast("\nINFO: Initializing Modules...\n");
  }

  // If Serial is Down, Use Bluetooth
  if(!SERIAL_ENABLED)
  {
    broadcast("WARNING: Unable to init Serial.\n");
  }

  // If Bluetooth is down, Use Serial
  if(!BLUETOOTH_ENABLED)
  {
    broadcast("WARNING: Unable to init Bluetooth.\n");
  }

  // Attempt to Init Datalogger
  if(initDatalogger())
  {
    DATALOGGER_ENABLED = false;
    broadcast("WARNING: Unable to init Datalogger.\n");
  }
  else
  {
    DATALOGGER_ENABLED = true;
  }

  // Attempt to Init INA260 (Critical Component)
  if(initINA260())
  {
    INA260_ENABLED = false;
    broadcast("WARNING: Unable to init INA260.\n");
  }
  else
  {
    INA260_ENABLED = true;
  }

  // Attempt to Init RTC
  if(initRTC())
  {
    RTC_ENABLED = false;
    broadcast("WARNING: Unable to init RTC.\n");
  }
  else
  {
    RTC_ENABLED = false;
  }

  broadcast("INFO: Initialization Complete!\n");

  // Return Success
  return 0;
}

int commandHandler()
{
  // User Input Variable
  static int input;

  // Print Main Menu Options
  broadcast("\nMAIN MENU: \n");
  broadcast(" 1. RECORD DATA \n");
  broadcast(" 2. READ FILE \n");
  broadcast(" 3. MODULE STATUS \n");
  broadcast(" 4. RESET DEVICE \n");
  broadcast(" 5. INFORMATION \n");

  while(1)
  {
    input = getInt();

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
      static long duration;
      if(getDuration(duration))
      {

      }
      else if(recordData(duration))
      {

      }
      else
      {

      }
      return 0;
    }

    // Read File
    else if(input == 2)
    {
      static int filenumber;
      if(getFilenumber(filenumber))
      {
        
      }
      else if(printFile(filenumber))
      {
        
      }
      else
      {

      }
      return 0;
    }

    // Module Status
    else if(input == 3)
    {
      if(printStatus())
      {

      }
      else
      {

      }
      return 0;
    }

    // Reset Device
    else if(input == 4)
    {
      if(initModules())
      {

      }
      else
      {

      }
      return 0;
    }

    // Information
    else if(input == 5)
    {
      if(printInfo())
      {

      }
      else
      {

      }
      return 0;
    }

    // Invalid Entry
    else
    {
      broadcast("\nERROR: Invalid entry '"+String(input)+"'.\n");
      return 0;
    }
  }

  // Return Success
  return 0;
}

int recordData(long duration)
{
  // Check INA260
  if(!INA260_ENABLED)
  {
    broadcast("\nERROR: Unable to find INA260.\n");
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
      broadcast("\nERROR: Unable to open file '");
      broadcast(charName);
      broadcast("'\n");
      return -1;
    }
    else
    {
      broadcast("\nINFO: Logging data to '"+String(charName)+"'\n");
    }
  }
  else // Without Dataloger
  {
    broadcast("\nWARNING: Starting test without datalogger.\n");
    broadcast("WARNING: Make sure to save data after running.\n");
  }

  broadcast("\nINFO: Logging data for '"+String(duration)+"' seconds.\n");
  broadcast("\nINFO: Enter a number to stop recording data early.\n\n");

  // Log Start Time
  String stamp = "START: ";
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

  // Loop Time Variables
  long start = getTime();
  long last = start;
  long now = start;

  // Data Variables
  float current, voltage, power, battery;
  String csv = "TIME,CURRENT,VOLTAGE,POWER,BATTERY";

  if(DATALOGGER_ENABLED)
  {
    log.println(csv);    
  }
  broadcast(csv+"\n");

  // Polling Time Loop
  while(now - start < duration)
  {
    //  Check for User Input
    if(getInt() > 0)
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

      csv = String(now)+","+String(current)+","+String(voltage)+","+String(power)+","+String(battery);

      // Log Data
      if(DATALOGGER_ENABLED)
      {
        log.println(csv);
        // log.print(now);
        // log.print(',');
        // log.print(current);
        // log.print(',');
        // log.print(voltage);
        // log.print(',');
        // log.print(power);
        // log.print(',');
        // log.print(battery);
        // log.print('\n');
      }

      broadcast(csv+"\n");
      

//      // Print Data to Serial
//      if(SERIAL_ENABLED)
//      {
//        Serial.println(csv);
//        // Serial.print(now);
//        // Serial.print(',');
//        // Serial.print(current);
//        // Serial.print(',');
//        // Serial.print(voltage);
//        // Serial.print(',');
//        // Serial.print(power);
//        // Serial.print(',');
//        // Serial.print(battery);
//        // Serial.print('\n');
//      }
//
//      // Print Data to Bluetooth
//      if(BLUETOOTH_ENABLED)
//      {
//        if(ble.isConnected())
//        {
//          ble.println(csv);
//          // ble.print(now);
//          // ble.print(',');
//          // ble.print(current);
//          // ble.print(',');
//          // ble.print(voltage);
//          // ble.print(',');
//          // ble.print(power);
//          // ble.print(',');
//          // ble.print(battery);
//          // ble.print('\n');
//          ble.waitForOK();
//        }
//      }

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

int printFile(int filenumber)
{
   // Check for Errors
  if(!DATALOGGER_ENABLED)
  {
    broadcast("\nERROR: Datalogger not available.\n");
    return -1;
  }
  
  File root = SD.open("/");
  if(!root)
  {
    broadcast("\nERROR: Unable to open root.");
    return -1;
  }

  char* filename;
  File entry;
  int entryNumber = 1;

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
      if(filenumber == entryNumber)
      {

        broadcast("\nFILENAME: "+String(entry.name())+"\n");
        
        printFile(entry);
        
        break;
      }

      // Try Next File
      else
      {
        entryNumber++;
      }
    }
  }

  root.close();

  return 0;
}

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
    broadcast("\nERROR: File not opened.\n");
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
  broadcast("\nMODULE STATUS: \n");

  broadcast("  SERIAL: "+String(SERIAL_ENABLED)+"\n");

  broadcast("  BLUETOOTH: "+String(BLUETOOTH_ENABLED)+"\n");

  broadcast("  DATALOGGER: "+String(DATALOGGER_ENABLED)+"\n");

  broadcast("  RTC: "+String(RTC_ENABLED)+"\n");

  broadcast("  INA260: "+String(INA260_ENABLED)+"\n");

  broadcast("  BATTERY: "+String(getBatteryVoltage())+" V\n");

  return 0;
}

int printInfo()
{
  broadcast("\nINFORMATION: Enter numbers to navigate menus. \n");

  broadcast("  1. RECORD DATA - Logs data for selected duration.\n");

  broadcast("  2. READ FILE - Prints selected file contents to streams.\n");

  broadcast("  3. MODULE STATUS - Prints module status to streams.\n");

  broadcast("  4. RESET DEVICE - Restarts device, resetting modules.\n");

  broadcast("  5. INFORMATION - Prints menu information.\n");

  return 0;
}
