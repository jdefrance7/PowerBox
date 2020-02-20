#include "ble.h"

// Status Flag
bool BLUETOOTH_ENABLED = false;

// Bluetooth Object
Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_CS, BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST);

// Bluetooth Initialization
int initBluetooth()
{
  // Attempts communication with bluetooth module
  if(!ble.begin(VERBOSE_MODE))
  {
    BLUETOOTH_ENABLED = false;
    return -1;
  }

  // Checks for factory reset (should be disabled)
  if(FACTORYRESET_ENABLE)
  {
    if(!ble.factoryReset())
    {
      BLUETOOTH_ENABLED = false;
      return -1;
    }
  }

  // Turns off echo for send/recieve
  ble.echo(false);

  // Turns off verbose output mode
  ble.verbose(false);

  // Checks bluetooth module version for LED behavior
  if(ble.isVersionAtLeast(MINIMUM_FIRMWARE_VERSION))
  {
    ble.sendCommandCheckOK("AT+HWModeLED=" MODE_LED_BEHAVIOUR);
  }

  // Sets bluetooth module to data mode
  ble.setMode(BLUEFRUIT_MODE_DATA);

  BLUETOOTH_ENABLED = true;
  return 0;
}
