// Variable que almacena el valor leído del pin analógico (0 a 1023)
int SENSOR;

// Variable que almacenará la temperatura en grados Celsius
float TEMPERATURA;

void setup() {
  // Inicia la comunicación serial a 9600 baudios
  // Permite enviar datos al monitor serial del computador
  Serial.begin(9600);
}

void loop() {
  // Lee el valor analógico del pin A0 (sensor conectado aquí)
  // El valor va de 0 (0V) a 1023 (5V)
  SENSOR = analogRead(A0);

  // Convierte el valor leído a temperatura
  // Paso 1: (SENSOR * 5000.0) / 1023 → convierte a milivoltios (0 a 5000 mV)
  // Paso 2: /10 → el sensor LM35 entrega 10 mV por cada grado Celsius
  // Resultado: temperatura en °C
  TEMPERATURA = ((SENSOR * 5000.0) / 1023) / 10;

  // Envía la temperatura al monitor serial
  // El ", 1" indica que se muestre con 1 decimal
  Serial.println(TEMPERATURA, 1);

  // Espera 1 segundo antes de repetir la lectura
  delay(1000);
}