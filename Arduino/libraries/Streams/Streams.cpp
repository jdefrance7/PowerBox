#include "Streams.h"


// Prints string to available streams
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

// Prints char to available streams
void broadcast(char c)
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

// Returns char from available streams
char getChar()
{
  if(SERIAL_ENABLED)
  {
    if(Serial.available())
    {
      return Serial.read();
    }
  }
  if(BLUETOOTH_ENABLED)
  {
    if(ble.isConnected())
    {
      if(ble.available())
      {
        return ble.read();
      }
    }
  }
  return '\0';
}

// Returns int from available streams
long getInt()
{
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

// Returns long from available streams
long getLong()
{
  if(SERIAL_ENABLED)
  {
    if(Serial.available())
    {
      return long(Serial.parseInt());
    }
  }
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
