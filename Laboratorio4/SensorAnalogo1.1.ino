// Variable que almacena el valor leído del pin analógico (0 a 1023)
int SENSOR;

// Variable que guarda la temperatura en grados Celsius
float TEMPERATURA;

// Variable que acumula varias lecturas para promediar
float SUMA;

void setup() {
  // Inicia la comunicación serial a 9600 baudios
  Serial.begin(9600);
}

void loop() {

  // Inicializa la suma en 0 antes de empezar a acumular valores
  SUMA = 0;

  // Ciclo que se repite 5 veces para tomar 5 lecturas
  for (int i = 0; i < 5; i++) {

    // Lee el valor del sensor en el pin A0 (0 a 1023)
    SENSOR = analogRead(A0);

    // Convierte la lectura a temperatura en °C
    // 1. Convierte a milivoltios (0 - 5000 mV)
    // 2. Divide entre 10 (LM35: 10 mV = 1°C)
    TEMPERATURA = ((SENSOR * 5000.0) / 1023) / 10;

    // Acumula la temperatura en la variable SUMA
    SUMA = TEMPERATURA + SUMA;

    // Espera 0.5 segundos antes de la siguiente lectura
    delay(500);
  }

  // Calcula el promedio de las 5 lecturas
  // SUMA/5.0 asegura que la división sea decimal
  // Muestra el resultado con 1 decimal en el monitor serial
  Serial.println(SUMA / 5.0, 1);
}