#include <SoftwareSerial.h> // Librería que facilitará la comunicación

SoftwareSerial mySerial(10, 11); // RX, TX 

const int ledCom = 6;

bool esperandoCom = false;  // Indica si estamos esperando mensajes del satélite (se activa al recibir el primero)
long nextTimeoutCom;  // Momento (en ms) en que se da por perdida la comunicación si no llega ningún mensaje

void setup() {
  mySerial.begin(9600);
  Serial.begin(9600);

  pinMode(ledCom, OUTPUT);  // Configura el LED de alarma de comunicación como salida

}

void loop() {
  if (mySerial.available()) { // mySerial.available() indica si se han recibido nuevos bytes por el canal serie
    String data = mySerial.readStringUntil('\n'); // En caso afirmativo se leen todos los bytes hasta el carácter de fin de línea, usando la función mySerial.readString()
    Serial.println(data);

    // Alarma de comunicación: al recibir cualquier mensaje se reinicia el timeout de 5 segundos y se apaga la alarma
    esperandoCom = true;
    nextTimeoutCom = millis() + 5000;
    digitalWrite(ledCom, LOW);

  }

  // Si pasan 5 segundos sin recibir ningún mensaje del satélite, se enciende el LED de comunicación y se avisa a la interfaz
  if (esperandoCom && (millis() >= nextTimeoutCom)) {
    digitalWrite(ledCom, HIGH);
    Serial.println("Fallo de comunicacion");
    esperandoCom = false;  // * Así el aviso se envía solo una vez
  }

}