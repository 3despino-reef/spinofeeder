// LedManager.cpp
#include "LEDManager.h"
#include <config.h>

void setupLED() {
  pinMode(RED_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);

  // Apagar todos los pines inicialmente
  digitalWrite(RED_PIN, LOW);
  digitalWrite(BLUE_PIN, LOW);
  digitalWrite(GREEN_PIN, LOW);
  
}

// Función para controlar el LED
void updateLEDStatus(String feederMode, byte totalFeedings, byte maxFeedings, bool wifiState, bool systemInTime) {

  // Caso 1: Si se han superado el número de dosificaciones, LED en rojo 
  if (totalFeedings>=maxFeedings) {
    digitalWrite(RED_PIN, HIGH); 
    digitalWrite(GREEN_PIN, LOW);
    digitalWrite(BLUE_PIN, LOW);
    vTaskDelay(400 / portTICK_PERIOD_MS);
  }
  // Caso 2: Si wifiState está en false, se pone el azul fijo
  else if (!wifiState) {
    digitalWrite(RED_PIN, LOW); 
    digitalWrite(GREEN_PIN, LOW);
    digitalWrite(BLUE_PIN, HIGH);
    vTaskDelay(200 / portTICK_PERIOD_MS); 
  }
  // Caso 3: Si systemInTime es false, se pone azul a parpadear
  else if (!systemInTime) {
    digitalWrite(RED_PIN, LOW);
    digitalWrite(GREEN_PIN, LOW);
    digitalWrite(BLUE_PIN, HIGH);
    vTaskDelay(100 / portTICK_PERIOD_MS);  
    digitalWrite(BLUE_PIN, LOW);
    vTaskDelay(300 / portTICK_PERIOD_MS);  
  }
  // Caso 4: Si no ocurre nada de lo anterior, el LED parpadea en verde (como una especie de heartbeat)
  else {
    digitalWrite(RED_PIN, LOW);
    digitalWrite(BLUE_PIN, LOW);
    digitalWrite(GREEN_PIN, HIGH);
    vTaskDelay(100 / portTICK_PERIOD_MS);  
    digitalWrite(GREEN_PIN, LOW);
    vTaskDelay(300 / portTICK_PERIOD_MS);
  }
}

void updateLEDEvent() {


    digitalWrite(RED_PIN, HIGH); 
    vTaskDelay(100 / portTICK_PERIOD_MS); 
    digitalWrite(RED_PIN, LOW); 
    vTaskDelay(100 / portTICK_PERIOD_MS); 
    digitalWrite(RED_PIN, HIGH); 
    vTaskDelay(100 / portTICK_PERIOD_MS); 
    digitalWrite(RED_PIN, LOW); 

  
}