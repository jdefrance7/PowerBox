#ifndef BLE_H
#define BLE_H

/*
  Library Wrapper for Adafruit_BluefruitLE_SPI Module

  Provides:
    - Adafruit_BluefruitLE_SPI Object (ble)
    - Bluetooth Status Flag (BLUETOOTH_ENABLED)
    - Initialization Function (initBluetooth())
*/

// Arduino Library
#include <Arduino.h>

// BlueFruitSPI Friend Libraries
#include "Adafruit_BLE.h"
#include "Adafruit_BluefruitLE_SPI.h"

// BlueFruitSPI Friend Globals
#define BUFSIZE                     128
#define VERBOSE_MODE                true
#define BLUEFRUIT_SPI_CS            A5
#define BLUEFRUIT_SPI_IRQ           A4
#define BLUEFRUIT_SPI_RST           -1
#define FACTORYRESET_ENABLE         0 // {0:Run 1:Test}
#define MINIMUM_FIRMWARE_VERSION    "0.6.6"
#define MODE_LED_BEHAVIOUR          "MODE"

// Status Flag
extern bool BLUETOOTH_ENABLED;

// BlueFruitSPI Friend Object
extern Adafruit_BluefruitLE_SPI ble;

// Bluetooth Initializer
int initBluetooth();

#endif // BLE_H
