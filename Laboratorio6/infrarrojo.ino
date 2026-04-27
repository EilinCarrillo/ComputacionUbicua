int SENSOR = 2;
int VALOR;
int ANTERIOR = 1;

void setup(){
  Serial.begin(9600);
  pinMode(SENSOR, INPUT);
}

void loop(){
  VALOR = digitalRead(SENSOR);
  if(VALOR != ANTERIOR){
    if(VALOR == HIGH)
    Serial.println("linea");
  else
  Serial.println("fuera");
  ANTERIOR = VALOR;
  }
  delay(500);
}