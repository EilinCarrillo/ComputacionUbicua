int PULSADOR = 2;
int LED = 3;
int ESTADO = LOW;

void setup() {
  pinMode(PULSADOR, INPUT);       
  pinMode(LED, OUTPUT);           
 }

 void loop(){
  while(digitalRead(PULSADOR) == LOW ) {  
  }
  ESTADO = digitalRead(LED);             
  digitalWrite(LED, !ESTADO);            

  
  while(digitalRead(PULSADOR) == HIGH ) {   

  }
 }


 //Simulador del primer laboratorio en tinkercad 
url="https://www.tinkercad.com/things/364UgtcIGdp-laboratoriopulsador?sharecode=0xZjyjnPLZFshVg1f1l4drz7Jps63yqBLbRMfXyrk4g"