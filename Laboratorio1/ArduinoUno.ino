void setup() {
  pinMode(13, OUTPUT);   // Configura el pin 13 como salida
}

void loop() {
  digitalWrite(13, HIGH); // Enciende el LED conectado al pin 13
  delay(1000);           // Espera 1 segundo
  digitalWrite(13, LOW); // Apaga el LED
  delay(1000);           // Espera 1 segundo
}