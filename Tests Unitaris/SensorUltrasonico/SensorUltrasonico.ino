// Pines del sensor ultrasónico HC-SR04
#define TRIGPIN 8  // Pin de disparo (emite el pulso)
#define ECHOPIN 9  // Pin de eco (recibe el rebote)

long nextMillis = 0;         // Marca de tiempo para la próxima lectura
const long interval = 1000;  // Intervalo de 1 segundo (1000 ms) entre lecturas

// Variables para controlar el disparo sin bloqueos ni delays
int estadoUltrasonico = 0;         // Controla en qué fase del disparo estamos (0, 1 o 2)
unsigned long nextMicrosTrig = 0;  // Marca de tiempo en microsegundos para el pulso

void setup() {
  Serial.begin(9600);         // Inicia comunicación serie con el PC
  pinMode(TRIGPIN, OUTPUT);   // Configura Trig como salida
  pinMode(ECHOPIN, INPUT);    // Configura Echo como entrada
  digitalWrite(TRIGPIN, LOW); // Asegura que Trig empiece apagado

  nextMillis = millis() + interval;  // Programa la primera medición en 1 segundo
}

void loop() {
  // PASO 1: Esperar 1 segundo y preparar el pin Trig en LOW
  if (estadoUltrasonico == 0 && millis() >= nextMillis) {
    digitalWrite(TRIGPIN, LOW);     // Limpia el pin antes de disparar
    nextMicrosTrig = micros() + 2;  // Programa esperar 2 microsegundos
    estadoUltrasonico = 1;          // Pasa al siguiente paso

  // PASO 2: Pasados 2 microsegundos, lanzar el pulso en HIGH
  } else if (estadoUltrasonico == 1 && micros() >= nextMicrosTrig) {
    digitalWrite(TRIGPIN, HIGH);     // Inicia el pulso ultrasónico
    nextMicrosTrig = micros() + 10;  // Programa mantener el pulso 10 microsegundos
    estadoUltrasonico = 2;           // Pasa al paso final

  // PASO 3: Pasados 10 microsegundos, cortar el pulso, medir e imprimir
  } else if (estadoUltrasonico == 2 && micros() >= nextMicrosTrig) {
    digitalWrite(TRIGPIN, LOW);  // Termina el pulso ultrasónico

    long duracion = pulseIn(ECHOPIN, HIGH, 30000);  // Mide cuánto tarda en volver el eco (en us)
    float d = (duracion * 0.0343) / 2.0;            // Convierte el tiempo en distancia (en cm)

    // Comprueba si el objeto está fuera del rango válido del sensor (2 a 400 cm)
    if (duracion == 0 || d < 2.0 || d > 400.0) {
      Serial.println("Fuera de rango");  // Avisa si no hubo rebote o está muy lejos/cerca
    } else {
      Serial.print("Distancia: ");  // Imprime etiqueta en el Monitor Serie
      Serial.print(d);              // Imprime el valor numérico calculado
      Serial.println(" cm");        // Imprime la unidad y hace salto de línea
    }

    estadoUltrasonico = 0;             // Reinicia el ciclo para la siguiente lectura
    nextMillis = millis() + interval;  // Programa la próxima medición dentro de 1 segundo
  }
}