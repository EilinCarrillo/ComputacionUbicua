int PULSADOR = 2;      
int LED = 3;           
int ESTADO = LOW;      

void setup() {
  pinMode(PULSADOR, INPUT);   // Pulsador como entrada
  pinMode(LED, OUTPUT);       // LED como salida
}

void loop() {
  while (digitalRead(PULSADOR) == LOW) { 
    // Espera hasta presionar botón
  }

  ESTADO = digitalRead(LED);     // Lee estado actual del LED
  digitalWrite(LED, !ESTADO);    // Cambia estado del LED

  while (digitalRead(PULSADOR) == HIGH) {
    // Espera hasta soltar botón
  }
}