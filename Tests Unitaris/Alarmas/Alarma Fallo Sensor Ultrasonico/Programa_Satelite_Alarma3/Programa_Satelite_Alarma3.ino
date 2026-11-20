#include <SoftwareSerial.h>  // Librería que facilitará la comunicación
#include <Servo.h>           // Librería para controlar servomotores

Servo miServo; // Llamamos al servomotor miServo

SoftwareSerial mySerial(10, 11);  // RX, TX

bool esperandoTimeoutDist = false;  // Indica si el ultrasónico lleva fallando o fuera de rango
long nextTimeoutDist = 0;           // Marca de tiempo (en ms) límite antes de lanzar la alarma (código 13)

// Pin de señal PWM para el servomotor
#define SERVOPIN 6
// Pines del sensor ultrasónico HC-SR04
#define TRIGPIN 8  // Pin de disparo (emite el pulso)
#define ECHOPIN 9  // Pin de eco (recibe el rebote)

// Variables de control del barrido del servo
int anguloActual = 0;            // Ángulo actual del servo (0 a 180 grados)
int pasoAngulo = 5;              // Grados que avanza en cada paso (positivo = ida, negativo = vuelta)
long nextMillisServo = 0;        // Marca de tiempo para el próximo paso
const long intervalServo = 100;  // Tiempo (en ms) entre cada paso del servo

// Variables de control del disparo ultrasónico
long nextMillisSensorU = 0;         // Marca de tiempo para la próxima lectura
const long intervalSensorU = 1000;  // Intervalo de 1 segundo (1000 ms) entre lecturas
int estadoUltrasonico = 0;          // Controla en qué fase del disparo estamos (0, 1 o 2)
long nextMicrosSensorU = 0;         // Marca de tiempo en microsegundos para el pulso

void setup() {
  Serial.begin(9600);    // Inicia la comunicación serie con el PC 
  mySerial.begin(9600);  // Inicia la comunicación serie con la estación de tierra

  miServo.attach(SERVOPIN);  // Asocia el servo al pin 6
  miServo.write(0);          // Posiciona el servo en 0 grados al arrancar
  
  pinMode(TRIGPIN, OUTPUT);    
  pinMode(ECHOPIN, INPUT);    
  digitalWrite(TRIGPIN, LOW);

  nextMillisSensorU = millis() + intervalSensorU;  // Programa la primera medición ultrasónica dentro de 1 segundo
  nextMillisServo = millis() + intervalServo;      // Programa el primer paso de giro del servo dentro de 100 ms
}

void loop() {
  // Movimiento constante del servomotor (Barrido de 0° a 180° y vuelta)
  if (millis() >= nextMillisServo) {
    anguloActual = anguloActual + pasoAngulo;       // Suma o resta el paso de grados según el sentido actual de giro
    if (anguloActual >= 180) {                      // Si alcanza o supera el tope superior de 180°
      anguloActual = 180;                           // Asegura que no exceda el límite físico del motor
      pasoAngulo = pasoAngulo * -1;                 // Invierte el signo del paso para comenzar a girar hacia 0°
    } else if (anguloActual <= 0) {                 // Si alcanza o baja del tope inferior de 0°
      anguloActual = 0;                             // Asegura que no baje de 0°
      pasoAngulo = pasoAngulo * -1;                 // Invierte el signo del paso para comenzar a girar hacia 180°
    }

    miServo.write(anguloActual);                    // Ordena al servomotor moverse al nuevo ángulo calculado
    nextMillisServo = millis() + intervalServo;     // Programa el siguiente incremento de ángulo dentro de 100 ms
  }

  // Lectura del sensor ultrasónico HC-SR04
  if (estadoUltrasonico == 0 && millis() >= nextMillisSensorU) {
    digitalWrite(TRIGPIN, LOW);         // Pone el pin Trig en bajo para asegurar un pulso limpio
    nextMicrosSensorU = micros() + 2;   // Programa una espera de 2 microsegundos
    estadoUltrasonico = 1;              // Avanza a la fase 1 de disparo

  } else if (estadoUltrasonico == 1 && micros() >= nextMicrosSensorU) {
    digitalWrite(TRIGPIN, HIGH);        // Activa el pin Trig para emitir la ráfaga ultrasónica
    nextMicrosSensorU = micros() + 10;  // Programa mantener el pulso activo durante 10 microsegundos
    estadoUltrasonico = 2;              // Avanza a la fase 2 de lectura del eco

  } else if (estadoUltrasonico == 2 && micros() >= nextMicrosSensorU) {
    digitalWrite(TRIGPIN, LOW);         // Finaliza el pulso ultrasónico poniendo Trig en bajo

    long duracion = pulseIn(ECHOPIN, HIGH, 30000);  // Mide el tiempo (en us) que tarda en volver el eco (máx. 30 ms)
    float d = (duracion * 0.0343) / 2.0;            // Calcula la distancia en cm (velocidad sonido 0.0343 cm/us, ida y vuelta)

    miServo.detach();  // Desvincula temporalmente el servo para evitar interferencias al transmitir

    if (duracion == 0 || d < 2.0 || d > 400.0) {    // Comprueba si no hubo eco (0) o está fuera de rango (2-400 cm)
      if (!esperandoTimeoutDist) {                  // Si es el primer fallo seguido, arranca el contador de 5 segundos
        esperandoTimeoutDist = true;
        nextTimeoutDist = millis() + 5000;          // 5 segundos de margen igual que el DHT11
      }
      if (millis() >= nextTimeoutDist) {            // Si lleva 5 segundos o más sin lecturas válidas
        mySerial.println("13:");                    // Código 13: alarma de fallo del sensor de distancia
      } else {                                      // Mientras no pasen los 5 segundos, envía distancia 0 en el ángulo actual
        mySerial.println("11:" + String(anguloActual) + ":0");
      }
    } else {                                        // Si la medición de distancia es válida
      esperandoTimeoutDist = false;                 // Cancela cualquier cuenta atrás de fallo del ultrasónico
      mySerial.println("11:" + String(anguloActual) + ":" + String(d));  // Envía código 11 con ángulo y distancia en cm
    }

    miServo.attach(SERVOPIN);     // Vuelve a conectar el servo al pin PWM
    miServo.write(anguloActual);  // Restaura inmediatamente la posición en la que estaba el servo

    estadoUltrasonico = 0;                           // Reinicia la máquina de estados del sensor ultrasónico
    nextMillisSensorU = millis() + intervalSensorU;  // Programa la próxima medición dentro de 1 segundo
  }
}