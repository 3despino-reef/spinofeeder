#ifndef CONFIG_H
#define CONFIG_H

// Pines relacionados con el motor
#define NEMA_STEP_PIN         26
#define NEMA_SLEEP_PIN        27
#define NEMA_DIR_PIN          33
#define PULSES_PER_DOSIFICATION 13 //pulsos por hueco (entre 13 y 14). (200 cada 360º)/15
#define DELAY_PULSES          40000 //delay antes de parar el motor. Define la velocidad 
// Variables de los LEDs indicadores  
#define BUTTON_PIN            12        
#define BLUE_PIN              14
#define GREEN_PIN             16
#define RED_PIN               17

// Dosificación
#define FEEDING_INTERVAL_CYCLE_HOURS 24  //cada cuantas horas se dosifica en modo ciclo
#define MAX_FEEDINGS                 14  //el número de huecos de comida 

// Variables de la conexión wifi
#define WIFI_SSID             "SSID";  //Sustituir por el nombre del wifi      
#define WIFI_PASSWORD         "PASSWORD"; //Sustituir por la contraseña del wifi 

// Configura la zona horaria de España (CET +1 y CEST +2)
#define CET_OFFSET 3600        // UTC +1 (horario estándar)
#define CEST_OFFSET 7200      // UTC +2 (horario de verano)
#define DST_START_MONTH 3     // Marzo (inicio de horario de verano)
#define DST_END_MONTH 10      // Octubre (fin de horario de verano)

#endif // CONFIG_H
