void setup() {
  pinMode(2, INPUT);   // Pin 2 como entrada
  pinMode(3, OUTPUT);  // Pin 3 como salida
}

void loop() {
  if (digitalRead(2) == HIGH) { // Si hay señal en pin 2
    digitalWrite(3, HIGH);      // Enciende salida en pin 3
  }
  else {
    digitalWrite(3, LOW);       // Apaga salida en pin 3
  }
}