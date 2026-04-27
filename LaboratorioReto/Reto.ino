#include <LiquidCrystal.h>
#include <DHT.h>

// LCD
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

// DHT11
#define DHTPIN 7
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// LM35
int pinLM35 = A0;

void setup() {
  lcd.begin(16, 2);
  dht.begin();
  Serial.begin(9600);
}

void loop() {

  // LM35
  int lectura = analogRead(pinLM35);
  float voltaje = lectura * (5.0 / 1023.0);
  float tempLM35 = voltaje * 100.0;

  // DHT11
  float tempDHT = dht.readTemperature();

  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("Temp_DHT11:");
  lcd.print(tempDHT);
  lcd.print("C");

  lcd.setCursor(0, 1);
  lcd.print("Temp_LM35:");
  lcd.print(tempLM35);
  lcd.print("C");

  delay(2000);
}