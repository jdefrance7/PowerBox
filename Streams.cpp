#include "Streams.h"

int getInt()
{
  // Check for Errors
  if(!SERIAL_ENABLED && !BLUETOOTH_ENABLED)
  {
    broadcast("ERROR: No streams available.\n");
    return -1;
  }

  // Check For Serial Data
  if(SERIAL_ENABLED)
  {
    if(Serial.available())
    {
      return int(Serial.parseInt());
    }
  }

  // Check For Bluetooth Data
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

  // Check for Serial Data
  if(SERIAL_ENABLED)
  {
    if(Serial.available())
    {
      return long(Serial.parseInt());
    }
  }

  // Check for Bluetooth Data
  if(BLUETOOTH_ENABLED)
  {
    if(ble.isConnected())
    {
      if(ble.available())
      {
        return long(ble.readline_parseInt());
      }
    }
  }

  return 0;
}

void broadcast(String msg)
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

void broadline(String msg)
{
  if(SERIAL_ENABLED)
  {
    Serial.println(msg);
  }
  if(BLUETOOTH_ENABLED)
  {
    if(ble.isConnected())
    {
      ble.println(msg);
      ble.waitForOK();
    }
  }
}
