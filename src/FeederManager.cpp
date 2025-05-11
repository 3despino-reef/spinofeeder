#include "FeederManager.h"


// Control de alimentación
unsigned long feedingIntervalCycle = FEEDING_INTERVAL_CYCLE_HOURS * 3600000UL;
unsigned long lastFeedingMillis    = 0;

// Flags de alimentación diaria
boolean fedAtFirstTime  = false;
boolean fedAtSecondTime = false;
int lastFeedingDay       = 0;

void setupFeederPins() {
  pinMode(NEMA_STEP_PIN, OUTPUT);
  pinMode(NEMA_DIR_PIN, OUTPUT);
  pinMode(NEMA_SLEEP_PIN, OUTPUT);

  digitalWrite(NEMA_SLEEP_PIN, LOW); // Deja desactivado el driver
  digitalWrite(NEMA_STEP_PIN, LOW);   // Estado inicial seguro
  digitalWrite(NEMA_DIR_PIN, LOW);    // Dirección fija (ajustable si necesitas invertirla)
}
void performFeedingStep() {
  //nos salimos si hemos llegado al número máximo de dosificaciones
  if (totalFeedings>=maxFeedings){
    return;
  }

  digitalWrite(NEMA_DIR_PIN, LOW);
  digitalWrite(NEMA_SLEEP_PIN, HIGH);

  for (int i = 0; i < PULSES_PER_DOSIFICATION; i++) {
    digitalWrite(NEMA_STEP_PIN, HIGH);
    digitalWrite(NEMA_STEP_PIN, LOW);
    delayMicroseconds(DELAY_PULSES);
  }

  digitalWrite(NEMA_SLEEP_PIN, LOW);
  lastFeedingMillis = millis(); //guardamos el momento de la última dosificación
  updateTotalFeeding(); //actualizamos el número de dosificaciones 

}

void handleFeedingLogic(String feederMode, boolean feedFirstTimeEnabled, boolean feedSecondTimeEnabled) {
  // Reinicia los flags si cambia el día. Evitamos que se dosifique todo el rato hasta que cambia el minuto 
  if (lastFeedingDay != day) {
    fedAtFirstTime = false;
    fedAtSecondTime = false;
  }

  if (feederMode == "cycle") {
    if (millis() - lastFeedingMillis >= feedingIntervalCycle) {
      performFeedingStep();
    }
  }

  if (feederMode == "auto") {
    //primera dosificación 
    if ((hour ==  feedHourFirst && minute ==  feedMinuteFirst) && !fedAtFirstTime && feedFirstTimeEnabled) {
      performFeedingStep();
      fedAtFirstTime = true;
      updateTotalFeeding();
    }
    //segunda dosificación
    if ((hour == feedHourSecond && minute == feedMinuteSecond) && !fedAtSecondTime && feedSecondTimeEnabled) {
      performFeedingStep();
      fedAtSecondTime = true;
      updateTotalFeeding();
    }
  }

}

void manualFeed(){
  performFeedingStep();
}

void updateTotalFeeding() {
  lastFeedingDay = day;
  totalFeedings++;
  preferences.begin("datos", false);
  preferences.putInt("totalFeedings", totalFeedings);
  preferences.end();

}

void resetTotalFeeding() {
  totalFeedings = 0;
  preferences.begin("datos", false);
  preferences.putInt("totalFeedings", totalFeedings);
  preferences.end();
}

int calculateHoursSinceFeeding() {
  unsigned long elapsedMillis = millis() - lastFeedingMillis;
  unsigned long hours = elapsedMillis / 3600000UL;
    // Limitar al máximo valor permitido por un `int` de 16 bits (32,767)
  if (hours > INT16_MAX) {
    return INT16_MAX; // o 32767 si no incluyes <limits.h>
  }

  return (int)hours;
}