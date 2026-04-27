#include <LiquidCrystal.h>

LiquidCrystal lcd(7, 6, 5, 4, 3, 2); // Pines del LCD
int SENSOR;                          
float TEMPERATURA;                   
float SUMA;                          

void setup() {
  lcd.begin(16, 2); // Inicia LCD de 16x2
}

void loop() {
  SUMA = 0; // Reinicia suma

  for (int i = 0; i < 5; i++) {
    SENSOR = analogRead(A0); // Lee sensor en A0
    TEMPERATURA = ((SENSOR * 5000.0) / 1023) / 10; // Convierte a °C
    SUMA = TEMPERATURA + SUMA; // Acumula lectura
    delay(500); // Espera medio segundo
  }

  lcd.setCursor(0, 0);     // Cursor fila 1 columna 1
  lcd.print("Temp: ");     
  lcd.print(SUMA / 5.0, 1); // Promedio con 1 decimal
  lcd.print(" C");         
}