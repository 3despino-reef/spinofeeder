// LedManager.h
#ifndef LEDMANAGER_H
#define LEDMANAGER_H

#include <Arduino.h> 

void setupLED();
void updateLEDStatus(String feederMode, byte totalFeedings, byte maxFeedings, bool wifiState, bool systemInTime);
void updateLEDEvent();

#endif