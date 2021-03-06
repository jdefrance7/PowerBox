#ifndef BLUETOOTH_H
#define BLUETOOTH_H

// Provides Adafruit_BluefruitLE_SPI Object (ble) and supporting init function.

// Arduino Library
#include <Arduino.h>

// BlueFruitSPI Friend Libraries
#include <Adafruit_BLE.h>
#include <Adafruit_BluefruitLE_SPI.h>

// BlueFruitSPI Friend Globals
#define BUFSIZE                     128
#define VERBOSE_MODE                true
#define BLUEFRUIT_SPI_CS            A5
#define BLUEFRUIT_SPI_IRQ           A4
#define BLUEFRUIT_SPI_RST           -1
#define FACTORYRESET_ENABLE         0 // {0:Run 1:Test}
#define MINIMUM_FIRMWARE_VERSION    "0.6.6"
#define MODE_LED_BEHAVIOUR          "MODE"

// BlueFruitSPI Friend Object
extern Adafruit_BluefruitLE_SPI ble;

// Bluetooth Flag
extern bool BLUETOOTH_ENABLED;

// Bluetooth Initializer
int initBluetooth();

#endif // BLUETOOTH_H
