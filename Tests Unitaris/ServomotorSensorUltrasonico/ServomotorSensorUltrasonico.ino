#include <Servo.h>  // Librería para controlar servomotores

Servo miServo; // Llamamos al servomotor miServo

// Pin de señal PWM para el servomotor
#define SERVOPIN 6  // Modificar el pin para hacer el test en los dos servomotores (pin 6 para Servo1, pin 5 para Servo2)
// Pines del sensor ultrasónico HC-SR04
#define TRIGPIN 8  // Pin de disparo (emite el pulso)
#define ECHOPIN 9  // Pin de eco (recibe el rebote)

// Variables de control del barrido del servo
int anguloActual = 0;            // Ángulo actual del servo (0 a 180 grados)
int pasoAngulo = 5;              // Grados que avanza en cada paso (positivo = ida, negativo = vuelta)
long nextMillisServo = 0;        // Marca de tiempo para el proximo paso
const long intervalServo = 100;  // Tiempo (en ms) entre cada paso del servo (controla la velocidad de giro)

// Variables de control del disparo ultrasónico
long nextMillisSensor = 0;         // Marca de tiempo para la próxima lectura
const long intervalSensor = 1000;  // Intervalo de 1 segundo (1000 ms) entre lecturas
int estadoUltrasonico = 0;         // Controla en qué fase del disparo estamos (0, 1 o 2)
long nextMicrosSensor = 0;         // Marca de tiempo en microsegundos para el pulso

void setup() {
  Serial.begin(9600);         // Inicia comunicación serie con el PC

  miServo.attach(SERVOPIN);  // Asocia el servo al pin 6
  miServo.write(0);          // Posiciona el servo en 0 grados al arrancar

  pinMode(TRIGPIN, OUTPUT);   // Configura Trig como salida
  pinMode(ECHOPIN, INPUT);    // Configura Echo como entrada
  digitalWrite(TRIGPIN, LOW); // Asegura que Trig empiece apagado

  nextMillisSensor = millis() + intervalSensor;  // Programa la primera medición en 1 segundo
  nextMillisServo = millis() + intervalServo;

}

void loop() {
  // Movimiento constante del servomotor (Barrido de 0° a 180° y vuelta)
  if (millis() >= nextMillisServo) {
    anguloActual = anguloActual + pasoAngulo;  // Avanza o retrocede según el signo de pasoAngulo

    // Invertir sentido de giro al llegar a los extremos (0° o 180°)
    if (anguloActual >= 180) {
      anguloActual = 180;            // Limita el ángulo al máximo físico del servo (180°) por si se cambia el valor de pasoAngulo
      pasoAngulo = pasoAngulo * -1;  // Cambia dirección hacia 0°
    } else if (anguloActual <= 0) {
      anguloActual = 0;              // Limita el ángulo al máximo físico del servo (0°) por si se cambia el valor de pasoAngulo
      pasoAngulo = pasoAngulo * -1;  // Cambia dirección hacia 180°
    }

    miServo.write(anguloActual);                 // Mueve el servo al nuevo ángulo
    nextMillisServo = millis() + intervalServo;  // Programa el siguiente paso
  }

  // Lectura del sensor ultrasónico
  // PASO 1: Esperar 1 segundo y preparar el pin Trig en LOW
  if (estadoUltrasonico == 0 && millis() >= nextMillisSensor) {
    digitalWrite(TRIGPIN, LOW);       // Limpia el pin antes de disparar
    nextMicrosSensor = micros() + 2;  // Programa esperar 2 microsegundos
    estadoUltrasonico = 1;            // Pasa al siguiente paso

  // PASO 2: Pasados 2 microsegundos, lanzar el pulso en HIGH
  } else if (estadoUltrasonico == 1 && micros() >= nextMicrosSensor) {
    digitalWrite(TRIGPIN, HIGH);     // Inicia el pulso ultrasónico
    nextMicrosSensor = micros() + 10;  // Programa mantener el pulso 10 microsegundos
    estadoUltrasonico = 2;           // Pasa al paso final

  // PASO 3: Pasados 10 microsegundos, cortar el pulso, medir e imprimir
  } else if (estadoUltrasonico == 2 && micros() >= nextMicrosSensor) {
    digitalWrite(TRIGPIN, LOW);  // Termina el pulso ultrasónico

    long duracion = pulseIn(ECHOPIN, HIGH, 30000);  // Mide cuánto tarda en volver el eco (en us)
    float d = (duracion * 0.0343) / 2.0;            // Convierte el tiempo en distancia (en cm)

    // Imprimir en el Monitor Serie formato: Angulo | Distancia
    Serial.print("Angulo: ");
    Serial.print(anguloActual);
    Serial.print(" deg | ");
    
    // Comprueba si el objeto está fuera del rango válido del sensor (2 a 400 cm)
    if (duracion == 0 || d < 2.0 || d > 400.0) {
      Serial.println("Fuera de rango");  // Avisa si no hubo rebote o está muy lejos/cerca
    } else {
      Serial.print("Distancia: ");  // Imprime etiqueta en el Monitor Serie
      Serial.print(d);              // Imprime el valor numérico calculado
      Serial.println(" cm");        // Imprime la unidad y hace salto de línea
    }

    estadoUltrasonico = 0;             // Reinicia el ciclo para la siguiente lectura
    nextMillisSensor = millis() + intervalSensor;  // Programa la próxima medición dentro de 1 segundo
  }
}