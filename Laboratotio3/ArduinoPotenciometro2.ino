int LED = 3;				
int BRILLO;
int POT = 0;				

void setup(){
	pinMode(LED, OUTPUT);	
	
}

void loop(){
	BRILLO = analogRead(POT) / 4;	
	analogWrite(LED, BRILLO);	
}

//Simulador del segundo laboratorio en tinkercad 
url="https://www.tinkercad.com/things/cRoQZJUasfq-laboratoriopotenciometro?sharecode=UCIaty1NZDFhsTirBbet9tZPRUryB8vMTUes_8ocuLA"