// --- Motor Shield L298P ---
// Motor A conectado a M1 (izquierdo)
// Motor B conectado a M2 (derecho)

// Pines Motor A
#define IN1 12
#define IN2 13
#define ENA 10

// Pines Motor B
#define IN3 8
#define IN4 9
#define ENB 11

// Pines receptor KRF100
#define D0 2  // Botón adelante
#define D1 3  // Botón atrás
#define D2 4  // Botón izquierda
#define D3 5  // Botón derecha

// --- Compensación de potencia ---
#define COMP_IZQ 10
#define COMP_DER 10

// --- VELOCIDAD BASE ---
#define VELOCIDAD_BASE 240

// ==============================
// AGREGADO: Sensor HC-SR04
#define TRIG_PIN 6
#define ECHO_PIN 7
#define DISTANCIA_MINIMA 20  // cm — si hay obstáculo más cerca, bloquea el avance
// ==============================

void setup() {
  Serial.begin(9600);

  // Configurar pines del motor
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENB, OUTPUT);

  // Configurar pines del receptor
  pinMode(D0, INPUT);
  pinMode(D1, INPUT);
  pinMode(D2, INPUT);
  pinMode(D3, INPUT);

  // AGREGADO: Configurar pines del ultrasonido
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  Serial.println("Sistema listo...");
}

// AGREGADO: Función para medir distancia en cm
long medirDistancia() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  long duration = pulseIn(ECHO_PIN, HIGH, 30000);
  long distancia = duration * 0.034 / 2;
  if (distancia == 0) distancia = 400; // Si falla la lectura, asume campo libre
  return distancia;
}

void loop() {
  // Leer señales del control remoto
  bool adelante = digitalRead(D0);
  bool atras = digitalRead(D1);
  bool izquierda = digitalRead(D2);
  bool derecha = digitalRead(D3);

  // AGREGADO: Solo medir distancia si se intenta avanzar
  if (adelante) {
    long distancia = medirDistancia();
    Serial.print("Distancia: ");
    Serial.print(distancia);
    Serial.println(" cm");

    if (distancia > DISTANCIA_MINIMA) {
      Adelante(); // Camino libre → avanza normal
    } else {
      Parar();    // Obstáculo detectado → se detiene aunque el botón esté presionado
      Serial.println(" Obstáculo detectado — bloqueado");
    }
  }
  // --- El resto del control queda exactamente igual ---
  else if (atras) {
    Atras();
  } 
  else if (izquierda) {
    Izquierda();
  } 
  else if (derecha) {
    Derecha();
  } 
  else {
    Parar();
  }
}

// --- FUNCIONES DE MOVIMIENTO --- (sin cambios)

void Adelante() {
  Serial.println(" Adelante");
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  analogWrite(ENA, limitar(VELOCIDAD_BASE + COMP_IZQ));

  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENB, limitar(VELOCIDAD_BASE + COMP_DER));
}

void Atras() {
  Serial.println("Atrás");
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  analogWrite(ENA, limitar(VELOCIDAD_BASE + COMP_IZQ));

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(ENB, limitar(VELOCIDAD_BASE + COMP_DER));
}

void Izquierda() {
  Serial.println("Izquierda");
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  analogWrite(ENA, 0);

  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENB, limitar(VELOCIDAD_BASE + COMP_DER));
}

void Derecha() {
  Serial.println("Derecha");
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  analogWrite(ENA, limitar(VELOCIDAD_BASE + COMP_IZQ));

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  analogWrite(ENB, 0);
}

void Parar() {
  Serial.println(" Parado");
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
}

// --- Función de seguridad para no pasar de 255 ---
int limitar(int valor) {
  if (valor > 255) return 255;
  if (valor < 0) return 0;
  return valor;
}