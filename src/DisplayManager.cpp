#include "DisplayManager.h"

// Crear objeto de la pantalla OLED
Adafruit_SSD1306 display(WIDTH_DISPLAY, HEIGHT_DISPLAY, &Wire, -1);

// Configuración inicial de la pantalla
void setupDisplay() {
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("Error al inicializar la pantalla OLED.");
    //salimos de la función si hay un error con la pantalla
    return;
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
}

// Actualizar la pantalla con los datos actuales
void updateDisplay(String feederMode, byte totalFeedings, byte maxFeedings, int hoursSinceFeeding, boolean feedFirstTimeEnabled, boolean feedSecondTimeEnabled, byte feedHourFirst, byte feedMinuteFirst, byte feedHourSecond, byte feedMinuteSecond, int hour, int minute, boolean wifiState) {

  display.clearDisplay();// limpia pantalla
  display.setCursor (0, 0);	
  display.print(hour); 	 
  display.print(":"); 	 
  display.print(minute); 	 

  display.setCursor (2, 12);		
  display.print("Dosificaciones:"); 	
  display.print(totalFeedings); 	
  display.print("/"); 
  display.print(maxFeedings); 

  display.setCursor (2, 23);		
  display.print("Dos.1 -- "); 
  if (feedHourFirst<10){display.print("0");}
  display.print(feedHourFirst); 
  display.print(":"); 	 
  if (feedMinuteFirst<10){display.print("0");}
  display.print(feedMinuteFirst); 
  display.setCursor (90, 23);
  if (feedFirstTimeEnabled==true){ display.print(" (on)");}
  else{display.print(" (off)");}
  display.setCursor (2, 34);		
  display.print("Dos.2 -- "); 
  if (feedHourSecond<10){display.print("0");}
  display.print(feedHourSecond); 
  display.print(":"); 	 
  if (feedMinuteSecond<10){display.print("0");}
  display.print(feedMinuteSecond);  
  display.setCursor (90, 34);
  if (feedSecondTimeEnabled==true){ display.print(" (on)");}
  else{display.print(" (off)");} 	 
  display.setCursor (2, 57);		
  display.print("Modo:"); 	 
  display.print(feederMode); 	
  display.setCursor (65, 57);	
  display.print("Last:");
  if (hoursSinceFeeding<1){display.print("<0");}
  else if (hoursSinceFeeding> 48){display.print(">48");}
  else{ display.print("hoursSinceFeeding");}	 
  display.print("h"); 
  display.display();			// muestra en pantalla todo lo establecido anteriormente

}


void showMenuCarrusel(String options[], byte numberOptions, byte selectedOption) {
  display.clearDisplay();
  // Manejo de los elementos anterior, actual y siguiente con límites circulares
  String previous = (selectedOption == 0) ? options[numberOptions - 1] : options[selectedOption - 1];
  String current  = options[selectedOption];
  String next     = (selectedOption == numberOptions - 1) ? options[0] : options[selectedOption + 1];

  // Mostrar el elemento anterior
  display.setCursor(10, 10);
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.print(previous);

  // Resaltar el elemento actual
  display.drawRect(5, 25, 110, 15, SSD1306_WHITE); 
  display.setCursor(10, 30);
  display.setTextSize(1);
  display.print(current);

  // Mostrar el siguiente elemento
  display.setCursor(10, 50);
  display.print(next);

  // Dibujar barra de desplazamiento lateral
  drawScrollbar(selectedOption, numberOptions);

  display.display();
}
void drawScrollbar(byte selectedOption, byte numberOptions) {
  byte barHeight = HEIGHT_DISPLAY / numberOptions;
  byte barY = selectedOption * barHeight;

  // Dibujar el fondo de la barra
  display.drawRect(120, 0, 7, HEIGHT_DISPLAY, SSD1306_WHITE);
  display.fillRect(121, barY, 5, barHeight, SSD1306_WHITE);
}




void showTimeAdjust(byte hour, byte minute, byte selectedOption, bool editing, bool checkboxEnabled) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  // Título
  display.setCursor(0, 0);
  display.print("Selecciona la hora");

  // Posiciones
  byte hourX = 20, hourY = 15;
  byte minuteX = 65, minuteY = 15;
  byte checkboxX = 10, checkboxY = 35;
  byte confirmX = 10, confirmY = 50;
  byte cancelX = 70, cancelY = 50;

  // Recuadro de selección
  if (selectedOption == 0) display.drawRect(hourX - 5, hourY - 5, 30, 15, SSD1306_WHITE);
  if (selectedOption == 1) display.drawRect(minuteX - 5, minuteY - 5, 30, 15, SSD1306_WHITE);
  if (selectedOption == 2) display.drawRect(checkboxX - 5, checkboxY - 5, 100, 15, SSD1306_WHITE);
  if (selectedOption == 3) display.drawRect(confirmX - 5, confirmY - 5, 50, 15, SSD1306_WHITE);
  if (selectedOption == 4) display.drawRect(cancelX - 5, cancelY - 5, 50, 15, SSD1306_WHITE);

  // Modo edición: fondo blanco para campo activo
  if (editing && selectedOption == 0) display.fillRect(hourX - 5, hourY - 5, 30, 15, SSD1306_WHITE);
  if (editing && selectedOption == 1) display.fillRect(minuteX - 5, minuteY - 5, 30, 15, SSD1306_WHITE);
  if (editing && selectedOption == 2) display.fillRect(checkboxX - 5, checkboxY - 5, 100, 15, SSD1306_WHITE);

  // Hora
  display.setCursor(hourX, hourY);
  if (editing && selectedOption == 0) display.setTextColor(SSD1306_BLACK);
  if (hour < 10) display.print("0");
  display.print(hour);
  display.setTextColor(SSD1306_WHITE);

  // Separador :
  display.setCursor(55, hourY);
  display.print(":");

  // Minuto
  display.setCursor(minuteX, minuteY);
  if (editing && selectedOption == 1) display.setTextColor(SSD1306_BLACK);
  if (minute < 10) display.print("0");
  display.print(minute);
  display.setTextColor(SSD1306_WHITE);

  // Casilla Activado
  display.setCursor(checkboxX, checkboxY);
  if (editing && selectedOption == 2) display.setTextColor(SSD1306_BLACK);
  display.print("Activado: ");
  display.setTextColor(SSD1306_WHITE);

  int boxBorderColor = (editing && selectedOption == 2) ? SSD1306_BLACK : SSD1306_WHITE;
  display.drawRect(checkboxX + 50, checkboxY - 2, 10, 10, boxBorderColor);
  if (checkboxEnabled) {
    int color = (editing && selectedOption == 2) ? SSD1306_BLACK : SSD1306_WHITE;
    display.drawLine(checkboxX + 51, checkboxY + 2, checkboxX + 53, checkboxY + 6, color);
    display.drawLine(checkboxX + 53, checkboxY + 6, checkboxX + 58, checkboxY - 1, color);
  }

  // Botones
  display.setCursor(confirmX, confirmY);
  display.print("Guardar");
  display.setCursor(cancelX, cancelY);
  display.print("Cancelar");

  display.display();
}




void showMessageSave() { 
  display.clearDisplay();

  // Texto de guardado
  display.setCursor(20, 10);
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.print("Guardando...");

  // Definir posiciones para la barra y el icono
  int saveIconWidth = 20;
  int saveIconHeight = 20;
  int saveIconX = 90; // Posición del disquete en X
  int saveIconY = 25;
  int progressBarX = 10; // Inicio de la barra
  int progressBarWidth = saveIconX - progressBarX - 5; // Ajustamos el ancho para no tocar el disquete
  int progressBarHeight = 4; // Ajuste fino del grosor
  
  // Centrar verticalmente la barra con el icono
  int progressBarY = saveIconY + (saveIconHeight - progressBarHeight) / 2;

  // Animación de icono de guardado
  for (int i = 0; i <= 5; i++) {
      int fillWidth = (progressBarWidth / 5) * i; // Incremento de la barra

      display.clearDisplay(); // Limpiar para evitar superposiciones
      display.setCursor(20, 10);
      display.print("Guardando...");

      drawSaveIcon(saveIconX, saveIconY); // Dibujar icono de disquete
      display.fillRect(progressBarX, progressBarY, fillWidth, progressBarHeight, SSD1306_WHITE); // Barra de progreso
      display.display();
      delay(200); // Pausa para animación
  }

  delay(500); // Espera antes de limpiar pantalla
  display.clearDisplay();
  display.display();
}

// Función para dibujar el icono de disquete
void drawSaveIcon(int x, int y) {
  // Cuadrado exterior del disquete
  display.drawRect(x, y, 20, 20, SSD1306_WHITE);

  // Cuadrado interno simulando la etiqueta del disquete
  display.fillRect(x + 4, y + 4, 12, 6, SSD1306_WHITE);

  // Zona de la muesca de guardado
  display.fillRect(x + 6, y + 15, 8, 3, SSD1306_WHITE); // Ajuste de la muesca
}

void showAbout() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  // Título centrado
  display.setCursor(0, 0);
  display.print("Acerca de");

  // Información de la versión
  display.setCursor(10, 15);
  display.print("Version: V0.1");

  // Autor
  display.setCursor(10, 28);
  display.print("Autor: 3Despino");

  // Email de contacto
  display.setCursor(10, 41);
  display.print("Contacto:");
  display.setCursor(10, 51);
  display.print("3despino@protonmail.com");

  display.display();
}


