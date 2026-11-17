#include <SoftwareSerial.h> // Librería que facilitará la comunicación

SoftwareSerial mySerial(10, 11); // RX, TX 

const int ledre = 13;  // LED en el pin 13 

// LEDs de alarma
const int ledFallo = 7;
const int ledCom = 6;

long nextMillisLed;  // Momento (en ms) en que se debe apagar el LED
bool ledEncendido = false;  // Indica si el LED está encendido esperando a ser apagado

bool esperandoCom = false;  // Indica si estamos esperando mensajes del satélite (se activa al recibir el primero)
long nextTimeoutCom;  // Momento (en ms) en que se da por perdida la comunicación si no llega ningún mensaje

void setup() {  
   
  mySerial.begin(9600);
  Serial.begin(9600);
 
  pinMode(ledre, OUTPUT);
  pinMode(ledFallo, OUTPUT);  // * Configura el LED de alarma de fallo del sensor como salida
  pinMode(ledCom, OUTPUT);  // Configura el LED de alarma de comunicación como salida

}

void loop() {
  // Apagar el LED cuando haya pasado el tiempo, sin bloquear el programa
    if (ledEncendido && millis() >= nextMillisLed) {
      digitalWrite(ledre, LOW);  // Apaga el LED
      ledEncendido = false;  // Marca el LED como apagado
    }

    if (mySerial.available()) { // mySerial.available() indica si se han recibido nuevos bytes por el canal serie
      String data = mySerial.readStringUntil('\n'); // En caso afirmativo se leen todos los bytes hasta el carácter de fin de línea, usando la función mySerial.readString()
      Serial.println(data);
      digitalWrite(ledre, HIGH);
      ledEncendido = true;  // Marca el LED como encendido para que el loop lo apague después
      nextMillisLed = millis() + 100;  // Programa el apagado del LED dentro de 100 ms

      // Alarma de fallo del sensor: enciende el LED si llega "Fallo", lo apaga cuando vuelven a llegar datos correctos
      if (data.indexOf("Fallo") >= 0) {
        digitalWrite(ledFallo, HIGH);
      } 
      else if (data.indexOf("T:") >= 0) {
        digitalWrite(ledFallo, LOW);
      }

      // Alarma de comunicación: al recibir cualquier mensaje se reinicia el timeout de 5 segundos y se apaga la alarma
      esperandoCom = true;
      nextTimeoutCom = millis() + 5000;
      digitalWrite(ledCom, LOW);
    }

    // * Si pasan 5 segundos sin recibir ningún mensaje del satélite, se enciende el LED de comunicación y se avisa a la interfaz
    if (esperandoCom && (millis() >= nextTimeoutCom)) {
      digitalWrite(ledCom, HIGH);
      Serial.println("Fallo de comunicacion");
      esperandoCom = false;  // * Así el aviso se envía solo una vez
    }

    // Reenviar ordres de python a l'arduino satèl·lit
    if (Serial.available()) {
      String mensajePC = Serial.readStringUntil('\n');
      mensajePC.trim(); // Elimina espacios y saltos de línea
      mySerial.print(mensajePC); // Envia la comanda al Satèl·lit per mySerial
      Serial.println("Se ha enviado la orde");
    }


}