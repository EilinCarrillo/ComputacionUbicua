int LED = 3;       
int BRILLO;        
int POT = 0;       

void setup() {
  pinMode(LED, OUTPUT);   // LED como salida
}

void loop() {
  BRILLO = analogRead(POT) / 4; // Lee potenciómetro y ajusta valor
  analogWrite(LED, BRILLO);     // Controla brillo del LED
}