#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include <Arduino.h>

// Definir constantes de la pantalla
#define WIDTH_DISPLAY 128
#define HEIGHT_DISPLAY 64

// Prototipos de funciones
void setupDisplay();

void updateDisplay(
  String feederMode,
  byte totalFeedings,
  byte maxFeedings,
  int hoursSinceFeeding,
  boolean feedFirstTimeEnabled,
  boolean feedSecondTimeEnabled,
  byte feedHourFirst,
  byte feedMinuteFirst,
  byte feedHourSecond,
  byte feedMinuteSecond,
  int hour,
  int minute,
  boolean wifiState
);

void showMenuCarrusel(String options[], byte numberOptions, byte selectedOption);
void drawScrollbar(byte selectedOption, byte numberOptions);

void showTimeAdjust(byte hour, byte minute, byte selectedOption, bool editing, bool checkboxEnabled);

void showMessageSave();
void drawSaveIcon(int x, int y);

void showAbout();

#endif // DISPLAY_MANAGER_H
