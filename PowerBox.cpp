#include "PowerBox.h"

int initModules()
{
  // Init Onboard LED
  initLED();

  // Attempt to Init Serial Module
  initSerial();

  // Attempt to Init Bluetooth Module
  initBluetooth();

  // Critical Error if Both Streams are Down
  if(!SERIAL_ENABLED && !BLUETOOTH_ENABLED)
  {
    errorLED();
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
    broadcast("WARNING: Unable to init Datalogger.\n");
  }

  // Attempt to Init INA260
  if(initINA260())
  {
    broadcast("WARNING: Unable to init INA260.\n");
  }

  // Attempt to Init RTC
  if(initRTC())
  {
    broadcast("WARNING: Unable to init RTC.\n");
  }

  broadcast("INFO: Initialization Complete!\n");
}

int moduleStatus()
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

int resetDevice()
{
  return initModules();
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
