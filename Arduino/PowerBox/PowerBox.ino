// Arduino Library
#include <Arduino.h>

// Custom Libraries
#include <Battery.h>
#include <Bluetooth.h>
#include <Datalogger.h>
#include <INA260.h>
#include <RTC.h>
#include <Serial.h>
#include <LED.h>
#include <Streams.h>

// Global state variable
typedef int STATE
#define STATE_INIT      0
#define STATE_MENU      1
#define STATE_LOGGING   2
#define STATE_READING   3
#define STATE_STATUS    4
STATE state = STATE_INIT;

// State function declarations
STATE initialization();
STATE menu();
STATE logging();
STATE reading();
STATE status();

// Global status variable
#define BLUETOOTH_MASK    0b00000001
#define DATALOGGER_MASK   0b00000010
#define INA260_MASK       0b00000100
#define RTC_MASK          0b00001000
#define SERIAL_MASK       0b00010000
#define CRITICAL_MASK     0b00000111
#define RESET_MASK        0b00000000
byte status = RESET_MASK;

void setup()
{
  // pass
}

void loop()
{
  switch(state)
  {
    case STATE_INIT:
      state = initialization();
      break;
    case STATE_MENU:
      state = menu();
      break;
    case STATE_LOGGING:
      state = logging();
      break;
    case STATE_READING:
      state = reading();
      break;
    case STATE_STATUS:
      state = status();
      break;
    default:
      state = initialization();
      break;
  }
}

// State 0 - Initialization
STATE initialization()
{
  // Reset module status
  status = RESET_MASK;

  // Primary Devices: Bluetooth, Datalogger, INA260
  {
    // Initialize Bluetooth module
    if(!initBluetooth())
    {
      status |= BLUETOOTH_MASK;
    }

    // Initialize Datalogger module
    if(!initDatalogger())
    {
      status |= DATALOGGER_MASK;
    }

    // Initialize INA260 sensor
    if(!initINA260())
    {
      status |= INA260_MASK;
    }
  }

  // Secondary Devices: RTC, Serial
  {
    // Initialize RTC module
    if(!initRTC())
    {
      status |= RTC_MASK;
    }

    // Initialize Serial module
    if(!initSerial())
    {
      status |= SERIAL_MASK;
    }
  }

  // If primary devices aren't initialized, toggle LED and try again.
  if(status & CRITICAL_MASK)
  {
    led.toggle();
    return STATE_INIT;
  }
  led.off();

  // Return to menu
  return STATE_MENU;
}

// State 1 - Menu
STATE menu()
{
  broadcast('\n');
  broadcast("Enter number associated with desired option.\n");
  broadcast("\n)";
  broadcast("Main Menu:\n");
  broadcast("  1. Logging\n");
  broadcast("  2. Reading\n");
  broadcast("  3. Status\n");
  broadcast("  4. Reset\n");

  // User input variable
  int input = 0;

  // Wait for user input
  while(input == 0)
  {
    // Get user input
    input = getInt();

    // Check for input
    if(input != 0)
    {
      broadcast('\n');
      broadcast("Received: "); broadcast(String(input)); broadcast('\n');

      // Logging
      if(input == 1)
      {
        return STATE_LOGGING;
      }

      // Reading
      else if(input == 2)
      {
        return STATE_READING;
      }

      // Status
      else if(input == 3)
      {
        return STATE_STATUS;
      }

      // Reset
      else if(input == 4)
      {
        return STATE_INIT;
      }
    }
  }
}

// State 2 - Logging
STATE logging()
{
  broadcast('\n');
  broadcast("Enter logging duration in seconds or negative number to return to main menu.\n");

  // Duration variable
  long duration = 0;

  // Wait for user input
  while(duration == 0)
  {
    // Get user input
    duration = getLong();

    // Check user input
    if(duration != 0)
    {
      broadcast('\n');
      broadcast("Received: "); broadcast(String(duration)); broadcast('\n');

      // Exit case
      if(duration < 0)
      {
        return STATE_MENU;
      }

      // Valid duration
      else if(duration > 0)
      {
        break;
      }
    }
  }

  broadcast('\n');
  broadcast("Enter logging interval in seconds or negative number to return to main menu.\n");

  // Interval variable
  long interval = 0;

  // Wait for user input
  while(interval == 0)
  {
    // Get user input
    interval = getLong();

    // Check user input
    if(interval != 0)
    {
      broadcast('\n');
      broadcast("Received: "); broadcast(String(duration)); broadcast('\n');

      // Exit case
      if(interval < 0)
      {
        return STATE_MENU;
      }

      // Invalid iterval
      else if(interval >= duration)
      {
        broadcast('\n');
        broadcast("ERROR: interval must be less than duration.\n");
        interval = 0;
      }

      // Valid interval
      else
      {
        break;
      }
    }
  }

  // Filename variable based on timestamp
  String filename = "/"+timestamp()+".txt";

  // Get unique filename
  while(SD.exists(filename))
  {
    filename = "/"+timestamp()+".txt";
  }

  // Open file
  File log = SD.open(filename, FILE_WRITE);

  // Check if file opened
  if(!log)
  {
    broadcast('\n');
    broadcast("ERROR: unable to open file '"); broadcast(filename); broadcast("'\ ");
    return STATE_MENU;
  }

  broadcast('\n');
  broadcast("Logging will exit when duration is complete, a user interrrupt occurs, or low battery is detected.\n");
  broadcast('\n');
  broadcast("Logging Information\n");
  broadcast("  Duration: ");  broadcast(String(duration));  broadcast('\n');
  broadcast("  Interval: ");  broadcast(String(interval));  broadcast('\n');
  broadcast("  Filename: ");  broadcast(String(filename));  broadcast('\n');
  broadcast('\n');

  // User interrrupt variable
  char user_interrupt = '\0';

  // Start time reference
  long start_time = millis();

  // Last log time reference
  long last_log_time = millis();

  // Log for duration
  while((millis() - start_time) < (duration / 1000))
  {
    // Log at interval
    if((millis() - last_log_time) >= (interval * 1000))
    {
      // Get values
      float voltage = ina260.readBusVoltage();
      float current = ina260.readCurrent();
      float power   = ina260.readPower();

      // Format line
      String line = "";
      line += timestamp();
      line += ", ";
      line += String(voltage);
      line += ", ";
      line += String(current);
      line += ", ";
      line += String(power);
      line += '\n';

      // Log line
      log.print(line);

      // Print line
      broadcast(line);

      // Update last log time reference
      last_log_time = millis();
    }

    // Check for user interrupt
    user_interrupt = getChar();
    if(user_interrupt != '\0')
    {
      broadcast('\n');
      broadcast("User interrupt detected!\n");

      // Close file
      log.close();

      // Return to menu
      return STATE_MENU;
    }

    // Check battery voltage
    if(getBatteryVoltage() < VBATLOW)
    {
      broadcast('\n');
      broadcast("Low battery detected!\n");

      // Close file
      log.close();

      // Return to menu
      return STATE_MENU;
    }
  }

  broadcast('\n');
  broadcast("Duration complete!\n");

  // Close file
  log.close();

  // Return to menu
  return STATE_MENU;
}

// State 3 - Reading
STATE reading()
{
  broadcast('\n');
  broadcast("Files: \n");

  // Open root directory
  File root = SD.open("/");

  // Get first entry
  File entry = root.openNextFile();

  // File entry number
  int entry_number = 1;

  // Iterate through entire root directory
  while(entry)
  {
    // Skip subfolders
    if(entry.isDirectory())
    {
      continue;
    }

    // Print file names and numbers
    else
    {
      broadcast("  "+String(entry_number)+". "+entry.name()+'\n');
    }

    // Get next file
    entry = root.openNextFile();
  }

  broadcast('\n');
  broadcast("Enter filenumber for reading or negative number to return to the main menu\n");

  // User input file number
  int file_number = 0;

  // Wait for user input
  while(file_number == 0)
  {
    // Get user input
    file_number = getInt();

    // Check user input
    if(file_number != 0)
    {
      broadcast('\n');
      broadcast("Received: "); broadcast(String(file_number)); broadcast('\n');

      // Exit case
      if(file_number < 0)
      {
        root.close();
        return STATE_MENU;
      }

      // Invalid file number
      else if(file_number > entry_number)
      {
        broadcast('\n');
        broadcast("ERROR: file number must be within range of entries or negative exit value.\n")
        file_number = 0;
      }

      // Valid file number
      else
      {
        break;
      }
    }
  }

  // Reset search variables
  entry_number = 1;
  root.rewindDirectory();
  entry = root.openNextFile();

  // Search through root for matching entry number
  while(entry)
  {
    // Entry found
    if(file_number == entry_number)
    {
      broadcast('\n');
      broadcast("Reading file: "); broadcast(entry.name()); broadcast('\n');

      // Print entire file
      while(entry.available())
      {
        broadcast(entry.read());
      }

      // Close file
      entry.close();

      break;
    }

    // Check next entry
    entry = root.openNextFile();
    entry_number++;
  }

  // Return to menu
  return STATE_MENU;
}

// State 4 - Status
STATE status()
{
  broadcast('\n');
  broadcast("Module Status")

  // Print modules' status (0 = enabled, 1 = disabled)

  broadcast("  Bluetooth: ");
  if(status & BLUETOOTH_MASK)
  {
    broadcast("Disabled\n");
  }
  else
  {
    broadcast("Enabled\n");
  }

  broadcast("  Datalogger: ");
  if(status & DATALOGGER_MASK)
  {
    broadcast("Disabled\n");
  }
  else
  {
    broadcast("Enabled\n");
  }

  broadcast("  INA260: ");
  if(status & INA260_MASK)
  {
    broadcast("Disabled\n");
  }
  else
  {
    broadcast("Enabled\n");
  }

  broadcast("  RTC: ");
  if(atatus & RTC_MASK)
  {
    broadcast("Disabled\n");
  }
  else
  {
    broadcast("Enabled\n");
  }

  broadcast("  Serial: ");
  if(status & SERIAL_MASK)
  {
    broadcast("Disabled\n");
  }
  else
  {
    broadcast("Enabled\n");
  }

  return STATE_MENU;
}
