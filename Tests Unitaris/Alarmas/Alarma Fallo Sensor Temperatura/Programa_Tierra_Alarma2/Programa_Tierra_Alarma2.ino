#include <SoftwareSerial.h> // Librería que facilitará la comunicación

SoftwareSerial mySerial(10, 11); // RX, TX 

const int ledFallo = 7;

void setup() {
  mySerial.begin(9600);
  Serial.begin(9600);

  pinMode(ledFallo, OUTPUT);  // Configura el LED de alarma de fallo del sensor como salida

}

void loop() {
  if (mySerial.available()) { // mySerial.available() indica si se han recibido nuevos bytes por el canal serie
    String data = mySerial.readStringUntil('\n'); // En caso afirmativo se leen todos los bytes hasta el carácter de fin de línea, usando la función mySerial.readString()
    Serial.println(data);

    // Alarma de fallo del sensor: enciende el LED si llega "Fallo", lo apaga cuando vuelven a llegar datos correctos
    if (data.indexOf("Fallo") >= 0) {
      digitalWrite(ledFallo, HIGH);
    } 
    else if (data.indexOf("T:") >= 0) {
      digitalWrite(ledFallo, LOW);
    }

}
}
