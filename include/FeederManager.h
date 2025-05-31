#ifndef FEEDER_MANAGER_H
#define FEEDER_MANAGER_H

#include <Arduino.h>
#include <Preferences.h>
#include <Config.h>


// Control de alimentación
extern byte         totalFeedings; //traemos desde el main el número de dosificaciones que llevamos
extern byte         maxFeedings;
extern int          lastFeedingDay;
extern byte         totalFeedings;

// Hora y día actuales
extern int          hour;
extern int          minute;
extern int          day;

// Horarios programados
extern byte         feedHourFirst   ; //hora a la que ocurre la primera dosificación 
extern byte         feedMinuteFirst ; //minuto en el que ocurre la primera dosificación 
extern byte         feedHourSecond  ; //hora a la que ocurre la segunda dosificación 
extern byte         feedMinuteSecond; //minuto en el que ocure la segunda dosificación

// Preferencias en memoria
extern Preferences  preferences;

// Funciones principales que exponemos 
void setupFeederPins(); 
void performFeedingStep();
void handleFeedingLogic(String feederMode, boolean feedFirstTimeEnabled, boolean feedSecondTimeEnabled, boolean systemInTime, byte maxFeedings, byte feedHourFirst, byte feedMinuteFirst, byte feedHourSecond, byte feedMinuteSecond);
void manualFeed();
void updateTotalFeeding();
void resetTotalFeeding();
int  calculateHoursSinceFeeding();

#endif // FEEDER_MANAGER_H
