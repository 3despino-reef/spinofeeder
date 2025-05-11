#include "MenuManager.h"


// Estados del menú
enum MenuState {
    MAIN_MENU,
    MANUAL_FEED,
    MODE_MENU,
    FIRST_FEED,
    SECOND_FEED,
    ABOUT
};


// Variables internas del sistema de menú
bool menuActive = false;
MenuState menuState;
byte selectedHour = 0;
byte selectedMinute = 0;
byte selectedOption = 0;
bool firstEntry = true;
bool editing = false;

// Variables de interacción del botón
unsigned long lastInteractionTime = 0;
const unsigned long menuTimeout   = 20000; // 15 segundos de inactividad
byte lastnumberShortClicks        = 0;  // Guarda el último número de pulsaciones cortas;

// Inicia el pulsador para controlar el menú
void setupMenuControls() {
    setupButton(); // Inicializa el botón
}

// Función para obtener el estado del menú
bool isMenuActive() {
    return menuActive;
}

//  Maneja la lógica de navegación del menú según los clics del botón
void processMenuNavigation() {
    checkButton(); // Actualiza los clics mediante EasyButton

    // Obtener el estado actual de clics de forma consistente
    int shortClicks = getShortClicksCount();
    int longClicks = getLongClicksCount();

    // Si el número de clics cortos ha cambiado, reiniciar el tiempo de inactividad
    if (shortClicks != lastnumberShortClicks) {
        lastInteractionTime = millis();
        lastnumberShortClicks = shortClicks;
    }

    // Si pulsación larga y no estamos en el menú → activamos menú
    if (longClicks > 0 && !menuActive) {
        menuActive = true;
        menuState = MAIN_MENU;
        lastInteractionTime = millis();
        editing = false;

        resetClicks(); // limpiamos para evitar heredar clics
        return; // importante: evitamos procesar clicks residuales y entrar a menuActive sin haber vuelto a leer los clicks
    }

    // Si ya estamos en el menú, procesamos con los valores actuales
    if (menuActive) {
        handleMenu(shortClicks, longClicks);
        checkMenuTimeout();
    }
}


//  Verifica si el menú debe cerrarse por inactividad
void checkMenuTimeout() {
    if (millis() - lastInteractionTime > menuTimeout) {
        Serial.println("Menú cerrado por inactividad");
        menuActive = false;
        resetClicks();
        firstEntry = true;
    }
}

// Manejo de la navegación del menú
void handleMenu(byte numberShortClicks, byte numberLongClicks) {
    switch (menuState) {
        case MAIN_MENU: handleMainMenu(getShortClicksCount(), getLongClicksCount()); break;
        case MANUAL_FEED: handleModeMenu(getShortClicksCount(), getLongClicksCount()); break;
        case MODE_MENU: handleModeMenu(getShortClicksCount(), getLongClicksCount()); break;
        case FIRST_FEED: handleFirstFeed(getShortClicksCount(), getLongClicksCount()); break;
        case SECOND_FEED: handleSecondFeed(getShortClicksCount(), getLongClicksCount()); break;
        case ABOUT: handleAbout(getShortClicksCount(), getLongClicksCount()); break; 
    }
}

// Manejo del menú principal
void handleMainMenu(byte numberShortClicks, byte numberLongClicks) {
    String options []= {"Dosificar ahora", "Reset dosifica.", "Modo", "Dosificacion 1", "Dosificacion 2", "Salir", "Acerca de"};
    byte numberOptions = sizeof(options) / sizeof(options[0]); // Calculamos el tamaño ANTES
    showMenuCarrusel(options, numberOptions, numberShortClicks);
    if (numberLongClicks == 1) {
        switch (numberShortClicks) {
            case 0: menuState = MANUAL_FEED; break;
            case 1: 
                totalFeedings = 0; //reseteamos el número de dosificaciones
                showMessageSave();
                menuActive = false;
                break;
            case 2: menuState = MODE_MENU; break;
            case 3: menuState = FIRST_FEED; break;
            case 4: menuState = SECOND_FEED; break;
            case 5: menuState = ABOUT; break;
        }
        resetClicks();
        return; // Salimos para no seguir usando los clics ya reseteados
    }
    if (numberShortClicks >= numberOptions){
        resetShortClicks();

    }
}

// Manejo del menú de modo de luz
void handleModeMenu(byte numberShortClicks, byte numberLongClicks) {
    String options[] = {"Auto", "Cycle", "Off", "Salir"};
    byte numberOptions = sizeof(options) / sizeof(options[0]);

    showMenuCarrusel(options, numberOptions, numberShortClicks);

    // Abrimos las preferencias solo si se va a guardar algo
    if (numberLongClicks == 1) {
        preferences.begin("datos", false);

        switch (numberShortClicks) {
            case 0:
                feederMode = "auto";
                preferences.putString("feederMode", feederMode);
                break;
            case 1:
                feederMode = "cycle";
                preferences.putString("feederMode", feederMode);
                break;
            case 2:
                feederMode = "off";
                preferences.putString("feederMode", feederMode);
                break;
        }

        preferences.end();
        resetClicks();
        showMessageSave();
        menuActive = false;
        return; // Evitamos seguir usando clicks ya reseteados
    }

    if (numberShortClicks >= numberOptions) {
        resetShortClicks();
    }
}


void handleFirstFeed(byte numberShortClicks, byte numberLongClicks) {
    // Al entrar por primera vez, cargamos los valores actuales desde memoria
    if (firstEntry) {
        selectedHour = feedHourFirst;
        selectedMinute = feedMinuteFirst;
        firstEntry = false;
    }

    // Mostramos la interfaz visual con los datos actuales
    showTimeAdjust(selectedHour, selectedMinute, selectedOption, editing, feedFirstTimeEnabled);

    // Si NO estamos en modo edición, interpretamos los clics como navegación entre campos
    if (!editing) {
        selectedOption = numberShortClicks;

        if (numberLongClicks == 1) {
            editing = true;
            resetClicks();
            return; // Evitamos usar valores reseteados en este mismo ciclo
        }

        if (numberShortClicks >= 5) {
            resetShortClicks();
        }

        return; // evitamos entrar al bloque de edición por error
    }

    // Si estamos editando un campo, aplicamos lógica según el campo seleccionado
    switch (selectedOption) {
        case 0: // Editar hora
            selectedHour = numberShortClicks;
            resetShortClicks(); // Importante: limpiamos tras aplicar
            if (selectedHour >= 24) selectedHour = 0;


            if (numberLongClicks == 1) {
                editing = false;
                resetClicks();
                return;
            }
            break;

        case 1: // Editar minutos (de 5 en 5)
            selectedMinute += numberShortClicks * 5;
            resetShortClicks(); // Importante: limpiamos tras aplicar
            if (selectedMinute >= 60) selectedMinute = 0;

            if (numberLongClicks == 1) {
                editing = false;
                resetClicks();
                return;
            }
            break;

        case 2: // Activar/desactivar
            feedFirstTimeEnabled = numberShortClicks % 2; // 0 → desactivado, 1 → activado, 2 → vuelve a 0, etc.
            
            if (numberLongClicks == 1) {
                editing = false;
                resetClicks();
                return;
            }
            break;
            break;
        
        case 3: // Guardar 
            preferences.begin("datos", false);
            preferences.putInt("feedHourF", selectedHour);
            preferences.putInt("feedMinuteF", selectedMinute);
            preferences.putInt("feedFirstTimeEnabled", feedFirstTimeEnabled);
            preferences.end();

            feedHourFirst = selectedHour;
            feedMinuteFirst = selectedMinute;

            showMessageSave();
            menuActive = false;
            firstEntry = true;
            resetClicks();
            return;

        case 4: // Cancelar sin guardar
            menuActive = false;
            firstEntry = true;
            resetClicks();
            return;
    }
}


void handleSecondFeed(byte numberShortClicks, byte numberLongClicks) {
    //cuando entramos, nos traemos los valores de la hora guardada en memoria 
   if (firstEntry) {
       selectedHour = feedHourSecond;
       selectedMinute = feedMinuteSecond;
       firstEntry = false;
   }
   showTimeAdjust(selectedHour, selectedMinute, selectedOption, editing, feedSecondTimeEnabled);
   //cuando entramos, nos traemos los valores de la hora guardada en memoria 

   //si no estamos editando ningún campo:
   if (!editing) {
       // Navegar entre Hora → Minuto → Activacion → Confirmar → Cancelar
       selectedOption = numberShortClicks;
       //si hacemos una pulsación larga, entramos en edición
       if (numberLongClicks == 1) {
           editing=true;
           resetClicks();
       }
    if (numberShortClicks >= 5){
        resetShortClicks();
        // nos volvemos a traer los valores de conteo de clicks ya reseteados
        numberShortClicks = getShortClicksCount();
    }
           
   } 
   //si hemos seleccionado un campo:
   else {
       switch (selectedOption) {
           case 0: 
               //estamos editando las horas
               //la hora de encendido será la guardada actualmente + el número de clicks 
               selectedHour = selectedHour + numberShortClicks;
               if (selectedHour >=24) selectedHour = 0;
               if (numberLongClicks == 1) {
                   //hemos hecho una pulsación larga y salimos de la edición
                   editing=false;
                   resetClicks();
               }
               break;
           case 1: 
               //estamos editando los minutos. Vamos de 5 en 5
               //el minuto de encendido será el guardado actualmente + el número de clicks 
               selectedMinute = selectedMinute + numberShortClicks*5;
               if (selectedMinute >=60) selectedMinute = 0;
               if (numberLongClicks == 1) {
                   //hemos hecho una pulsación larga y salimos de la edición
                   editing=false;
                   resetClicks();
               }
               break;
           case 2: //le hemos dado a activar/desactivar
               preferences.begin("datos", false);
               preferences.putInt("feedSecondTimeEnabled", feedSecondTimeEnabled); 
               preferences.end();
               resetClicks();
               menuActive = false; //salimos del menu
               firstEntry = true;
               showMessageSave();
               break;
           case 3: //le hemos dado a guardar
               preferences.begin("datos", false);
               preferences.putInt("feedHourS", selectedHour); 
               preferences.putInt("feedMinuteS", selectedMinute);
               preferences.end();
               feedHourSecond = selectedHour;
               feedMinuteSecond = selectedMinute;
               resetClicks();
               menuActive = false; //salimos del menu
               firstEntry = true;
               showMessageSave();
               break;
           case 4: 
               //nos salimos sin hacer nada
               resetClicks();
               menuActive = false;
               firstEntry = true;
           break;
       }
   }
}

// Manejo mostrar la versión y el contacti 
void handleAbout(byte numberShortClicks, byte numberLongClicks) {
    showAbout();

    if (numberLongClicks == 1) {
        resetClicks();
        showMessageSave();
        menuActive = false; // Cerrar menú
    }

}




  


   

