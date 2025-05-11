#ifndef MENU_MANAGER_H
#define MENU_MANAGER_H

#include <Arduino.h>
#include <Preferences.h>
#include <DisplayManager.h>
#include <ButtonManager.h>

// Variables externas necesarias
extern String feederMode;

extern byte feedHourFirst;
extern byte feedMinuteFirst;
extern byte feedHourSecond ;
extern byte feedMinuteSecond;
extern bool feedFirstTimeEnabled;
extern bool feedSecondTimeEnabled;

extern byte totalFeedings;

extern Preferences preferences;

// Funciones públicas
// Inicialización del menú
void setupMenuControls();
// Gestión de la navegación del menú
bool isMenuActive();
void processMenuNavigation();
void checkMenuTimeout();
// Funciones para manejar cada submenú
void handleMenu(byte numberShortClicks, byte numberLongClicks);
void handleMainMenu(byte numberShortClicks, byte numberLongClicks);
void handleModeMenu(byte numberShortClicks, byte numberLongClicks);
void handleFirstFeed(byte numberShortClicks, byte numberLongClicks);
void handleSecondFeed(byte numberShortClicks, byte numberLongClicks);
void handleAbout(byte numberShortClicks, byte numberLongClicks);


#endif // MENU_MANAGER_H
