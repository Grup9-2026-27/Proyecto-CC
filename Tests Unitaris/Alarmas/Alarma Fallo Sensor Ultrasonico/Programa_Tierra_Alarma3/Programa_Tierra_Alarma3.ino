#include <SoftwareSerial.h> // Librería que facilitará la comunicación

SoftwareSerial mySerial(10, 11); // RX, TX 

const int ledFalloDist = 8;  // LED de alarma para el sensor de distancia

bool alarmaDist = false;    // Indica si la alarma del sensor de distancia ya está activada

void setup() {
  mySerial.begin(9600);  // Inicia la comunicación serie con el satélite a 9600 baudios
  Serial.begin(9600);    // Inicia la comunicación serie con el PC / Python a 9600 baudios

  pinMode(ledFalloDist, OUTPUT);
}

void loop() {
  // Recepción y procesamiento de telemetría procedente del satélite
  if (mySerial.available()) {                                   // Comprueba si han llegado datos desde el satélite
    String data = mySerial.readStringUntil('\n');               // Lee el string completo hasta el salto de línea
    data.trim();                                                // Elimina espacios y saltos de línea sobrantes
    Serial.println(data);                                       // Reenvía el string al PC (Python)
    
    // Alarma de fallo del sensor: enciende el LED si llega "13:", lo apaga cuando vuelven a llegar distancias válidas ("11:")
    if (data.startsWith("13:")) {
      digitalWrite(ledFalloDist, HIGH);
    } else if (data.startsWith("11:")) {
      int segonDosPunts = data.indexOf(':', 3);  // Busca el segundo ':' que separa el ángulo de la distancia
      if (segonDosPunts != -1 && data.substring(segonDosPunts + 1) != "0") {
        digitalWrite(ledFalloDist, LOW);
      }
    }
  }
}
