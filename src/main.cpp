/*========================
  COMENTARIOS 
========================
VERSION:08/08/2024
*/

// MODOS DE FUNCIONAMIENTO:
//  on--> activación manual
//  cycle--> no se tienen en cuenta los datos mqtt ni la hora. Las luces funcionan de forma cíclica (modo de emergencia)
//  auto--> modo por defecto. Enciende y apaga a las horas indicadas

#include <Arduino.h>
#include <config.h>
#include <Preferences.h>
#include <OneWire.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <esp_task_wdt.h>
#include <WiFiManager.h>
#include <DisplayManager.h>
#include <MenuManager.h>
#include <LEDManager.h>
#include <FeederManager.h>


//  VARIABLES DE PREFERENCES (ALMACENAMIENTO)
Preferences preferences;

 // Variable que recoge el momento en el que se inicia el encendido en modo cycle
unsigned long       previousMillisCycle = 0; 

//variables para la alimentación 
byte         totalFeedings          = 0;
String       feederMode             = "auto";
byte         feedHourFirst          = 0; //hora a la que ocurre la primera dosificación 
byte         feedMinuteFirst        = 0; //minuto en el que ocurre la primera dosificación 
byte         feedHourSecond         = 0; //hora a la que ocurre la segunda dosificación 
byte         feedMinuteSecond       = 0; //minuto en el que ocure la segunda dosificación
int          lastFeedingDay         = 0;
boolean      feedFirstTimeEnabled   = true;
boolean      feedSecondTimeEnabled  = true;
byte         maxFeedings            = MAX_FEEDINGS; 


//VARIABLES WiFi y MQTT
boolean             mqttState        = false; // Estado de la conexión mqtt
boolean             wifiState        = false; // Estado de la conexión wifi  
unsigned long       lastPublishTime = 0;   // momento de la última publicacion
unsigned long       publishInterval = 5000; //Cada cuanto publicamos      
//VARIABLES PARA LA HORA
// Instanciamos el cliente NTP
WiFiUDP             ntpUDP;
NTPClient           timeClient(ntpUDP, "pool.ntp.org", CET_OFFSET); // Usamos la hora CET por defecto
String              date                 = "null"; //fecha que extraeremos del mensaje recibido
int                 hour                 = 0;
int                 minute               = 0;
int                 day                  = 0;
int                 month                = 0;
int                 year                 = 0;
int                 dayOfMonth           = 0;
bool                systemInTime         = false; //determina si el sistema está en hora

//VARIABLES PANTALLA
unsigned long       lastDisplayToggle = 0;
bool                showAlternateDisplay = false;

// VARIABLE WATCHDOG
#define WDT_TIMEOUT 40 // Valor en segundos que puede estar el ESP32 sin reiniciar el loop


//FUNCIÓN DEBUG PARA IMPRIMIR LOS VALORES POR LA CONSOLA
void printStatus() {
  Serial.print("Hora: ");
  if (systemInTime==false){
    Serial.println("error en el servidor NTP");
  }
  else{ 
    Serial.print(hour);
    Serial.print(":");
    Serial.print(minute);
    Serial.print(" Día del mes: ");
    Serial.println(dayOfMonth);
  }
  

  Serial.print("Conexion wifi: ");
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("No Conectado");
  } else {
    Serial.println("Conectado");
  }

}


bool isLastSunday(int month, int dayOfMonth, int year) {
  // Calculamos el día de la semana para el último día del mes
  tm timeinfo = { 0 };
  timeinfo.tm_year = year - 1900;
  timeinfo.tm_mon = month - 1;  // Mes es 0-11
  timeinfo.tm_mday = 1; // Primer día del mes
  mktime(&timeinfo); // Llenar la estructura tm

  // Día de la semana para el primer día del mes (0 - Domingo, 1 - Lunes, etc.)
  int firstDayOfWeek = timeinfo.tm_wday;

  // El último domingo se calcula como el día del mes del último domingo
  int lastSunday = 31 - (firstDayOfWeek + 6) % 7;
  
  // Verificamos si el día actual es el último domingo del mes
  return dayOfMonth == lastSunday;
}

// Función para ajustar la hora por horario de verano (CEST)
int adjustForDST(int rawHour, int dayOfMonth, int month, int year) {
  // Verificar si estamos en horario de verano (último domingo de marzo) o en horario estándar (último domingo de octubre)
  if (month == DST_START_MONTH && isLastSunday(month, dayOfMonth, year)) {
    return rawHour + 1;  // Ajustamos la hora al horario de verano (CEST)
  } else if (month == DST_END_MONTH && isLastSunday(month, dayOfMonth, year)) {
    return rawHour - 1;  // Regresamos a horario estándar (CET)
  } else {
    return rawHour;  // Si no es el día del cambio de hora, mantenemos la hora actual
  }
}
//FUNCIÓN PARA EL CONTROL DE LA HORA
void checkTime(){
  // Nos guardamos el resultado en una variable y una vez puesto en hora no comprobamos mas.
  // la propia libreria lo comprueba por nosotros cada minuto.
  // Si no tenemos red ni lo intentamos...
  if(!systemInTime && WiFi.isConnected()) {
    Serial.println("Comprobando servidor NTP.");
    systemInTime = timeClient.update();
    if(!systemInTime) {
      systemInTime = timeClient.forceUpdate();
    }
  }
  
  timeClient.update();
  date = timeClient.getFormattedDate();
  hour = timeClient.getHours();
  minute = timeClient.getMinutes();
  day = timeClient.getDay();

  // Formato: "YYYY-MM-DD HH:MM:SS"
  year = date.substring(0, 4).toInt();
  month = date.substring(5, 7).toInt();
  dayOfMonth = date.substring(8, 10).toInt();
  
  // Ajustar la hora según el horario de verano (si corresponde)
  hour = adjustForDST(hour, dayOfMonth, month, year);
}




void setupPreferences() {
  preferences.begin("datos", false); // false para lectura/escritura

  // Verificar y establecer valores predeterminados si no existen
  if (!preferences.getBool("initialized", false)) {
    // preferences.putString("feederMode", "auto");
    // preferences.putInt("feedHourF", 14); //por si acaso hay algun error, ponemos las 14h como hora de dosificación 
    // preferences.putInt("feedMinuteF", 0);
    // preferences.putInt("feedHourS", 0);
    // preferences.putInt("feedMinuteS", 0);
    // preferences.putBool("Feed1Enabled", true); //por si acaso hay algun error, ponemos la primera dosificación en true 
    // preferences.putBool("Feed2Enabled", false);
    // preferences.putInt("totalFeedings", 0);
    preferences.putInt("lastFeedingDay", 0);  // 
    preferences.putBool("initialized", true);  // Marca como inicializado
  }

  // Leer los valores desde memoria
  feederMode           = preferences.getString("feederMode", "auto");
  feedHourFirst        = preferences.getInt("feedHourF", 15); //por si acaso hay algun error, ponemos las 14h como hora de dosificación 
  feedMinuteFirst      = preferences.getInt("feedMinuteF", 0);
  feedHourSecond       = preferences.getInt("feedHourS", 0);
  feedMinuteSecond     = preferences.getInt("feedMinuteS", 0);
  feedFirstTimeEnabled = preferences.getBool("Feed1Enabled", true); //por si acaso hay algun error, ponemos la primera dosificación en true 
  feedSecondTimeEnabled= preferences.getBool("Feed2Enabled", false);
  totalFeedings        = preferences.getInt("totalFeedings", 0);
  lastFeedingDay       = preferences.getInt("lastFeedingDay", 0);  // Guardamos el último dia de dosificación para evitar dosificaciones accidentales si se reinicia el equipo

  preferences.end();
}



void setup() {
  Serial.begin(115200);
  esp_task_wdt_init(WDT_TIMEOUT, true); // Iniciamos el watchdog
  esp_task_wdt_add(NULL); // Añadimos la tarea del loop al Watchdog

  setupWiFi();    //Iniciamos la conexión wifi
  setupDisplay(); // Iniciamos la pantalla
  setupMenuControls(); // Iniciamos el control del menu (el botón)
  setupLED();     // Iniciamos el LED indicador
  setupFeederPins(); // configurar los pines del motor

  setupPreferences(); // Configura las preferencias iniciales


  timeClient.begin();
  timeClient.setTimeOffset(7200);

}

void loop() {
  esp_task_wdt_reset(); // Reset del watchdog en cada iteración del loop
  
  wifiState = checkWiFi();



  handleFeedingLogic(
    feederMode, 
    feedFirstTimeEnabled, 
    feedSecondTimeEnabled, 
    systemInTime, 
    maxFeedings, 
    feedHourFirst, 
    feedMinuteFirst, 
    feedHourSecond, 
    feedMinuteSecond 
  );
  processMenuNavigation();  
  checkTime();
  printStatus();
  updateLEDStatus(feederMode, totalFeedings, maxFeedings, wifiState, systemInTime);
  // Solo actualizar la pantalla si el menú NO está activo
  // Solo actualizar la pantalla si el menú NO está activo
  if (!isMenuActive()) {
    unsigned long currentMillis = millis();
    

  updateDisplay(
    feederMode, 
    totalFeedings, 
    maxFeedings, 
    calculateHoursSinceFeeding(), 
    feedFirstTimeEnabled, 
    feedSecondTimeEnabled, 
    feedHourFirst, 
    feedMinuteFirst, 
    feedHourSecond, 
    feedMinuteSecond, 
    hour, 
    minute, 
    wifiState);
}
}
