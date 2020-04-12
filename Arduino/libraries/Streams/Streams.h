#ifndef STREAMS_H
#define STREAMS_H

#include <Arduino.h>
#include <Serial.h>
#include <Bluetoooth.h>

void broadcast(char c);
void broadcast(String msg);

char getChar();
int getInt();
long getLong();

#endif // STREAMS_H
