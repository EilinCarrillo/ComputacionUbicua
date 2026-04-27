int LED = 3;       
int BRILLO;        

void setup() {
  pinMode(LED, OUTPUT);   // LED como salida
}

void loop() {
  for (BRILLO = 0; BRILLO < 256; BRILLO++) { 
    analogWrite(LED, BRILLO);   // Aumenta brillo
    delay(15);                  // Espera 15 ms
  }

  for (BRILLO = 255; BRILLO >= 0; BRILLO--) {
    analogWrite(LED, BRILLO);   // Disminuye brillo
    delay(15);                  // Espera 15 ms
  }
}