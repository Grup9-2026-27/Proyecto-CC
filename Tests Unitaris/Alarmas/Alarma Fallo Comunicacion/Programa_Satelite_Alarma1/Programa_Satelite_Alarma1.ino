#include <SoftwareSerial.h>  // Librería que facilitará la comunicación

SoftwareSerial mySerial(10, 11);  // RX, TX

// Configuración para el latido
long nextMillisHB;  // Momento (en ms) en que toca enviar el siguiente latido
const long intervalHB = 2000;  // Un latido cada 2 segundos (menor que el timeout de 5 s de tierra)

void setup() {
  Serial.begin(9600);
  mySerial.begin(9600);

  nextMillisHB = millis() + intervalHB;  // Programa el primer latido

}

void loop() {
  // Latido: se envía siempre
  if (millis() >= nextMillisHB) {
    mySerial.println("Latido");  // Mensaje de latido
    nextMillisHB = millis() + intervalHB;  // Programa el siguiente latido

}
}