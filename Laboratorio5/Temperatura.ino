#include <DHT.h>
#include <DHT_U.h>

int SENSOR = 2;       
int TEMPERATURA;      
int HUMEDAD;          

DHT dht(SENSOR, DHT11); // Sensor DHT11 en pin 2

void setup() {
  Serial.begin(9600); // Inicia monitor serial
  dht.begin();        // Inicia sensor
}

void loop() {
  TEMPERATURA = dht.readTemperature(); // Lee temperatura
  HUMEDAD = dht.readHumidity();       // Lee humedad

  Serial.print("Temperatura: ");
  Serial.print(TEMPERATURA);

  Serial.print(" Humedad: ");
  Serial.println(HUMEDAD);

  delay(500); // Espera medio segundo
}