#include <SoftwareSerial.h> // Librería que facilitará la comunicación

SoftwareSerial mySerial(10, 11); // RX, TX 

const int ledre = 13;  // LED en el pin 13 


void setup() {  
   
  mySerial.begin(9600);
  Serial.begin(9600);
 
  pinMode(ledre, OUTPUT);

}

void loop() {
    if (mySerial.available()) { //mySerial.available() indica si se han recibido nuevos bytes por el canal serie
      String data = mySerial.readString(); // En caso afirmativo se leen todos los bytes hasta el carácter de fin de línea, usando la función mySerial.readString()
      Serial.println(data);
      digitalWrite(ledre, HIGH);
      delay(100);
      digitalWrite(ledre, LOW);

    }

    // Reenviar ordres de python a l'arduino satèl·lit
    if (Serial.available()) {
      String mensajePC = Serial.readStringUntil('\n');
      mensajePC.trim(); // Elimina espacios y saltos de línea
      mySerial.print(mensajePC); // Envia la comanda al Satèl·lit per mySerial
      Serial.println("Se ha enviado la orde");
    }

}