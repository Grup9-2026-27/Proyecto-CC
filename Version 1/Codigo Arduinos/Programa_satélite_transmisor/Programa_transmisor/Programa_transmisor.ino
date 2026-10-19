#include <SoftwareSerial.h>  // Librería que facilitará la comunicación
#include <DHT.h>             //Librería para leer el sensor DHT
#define DHTPIN 2
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

SoftwareSerial mySerial(10, 11);  // RX, TX

long nextMillis1;
const long interval1 = 3000;  // 3 segundos para el sensor de H y T

const int ledtr = 13;  // LED en el pin 13

// Variable para controlar si el satélite tiene que enviar datos o no
bool enviarDatos = true;


void setup() {
  Serial.begin(9600);
  mySerial.begin(9600);
  dht.begin();
  nextMillis1 = millis() + interval1;  // Configuración para el tiempo del sensor

  pinMode(ledtr, OUTPUT);
}

void loop() {
  // Recibir y procesar las órdenes de tierra
  if (mySerial.available()) {
    String mensajePC = mySerial.readString();
    if (mensajePC == "Parar") {
      enviarDatos = false;
      Serial.println(mensajePC); // Imprime la orden recibida
    } else if (mensajePC == "Reanudar") {
      enviarDatos = true;
      Serial.println(mensajePC); // Imprime la orden recibida
    }
  }

  // Configuración del sensor de humedad y temperatura
  if (millis() >= nextMillis1) {
    if (enviarDatos == true) {
      float h = dht.readHumidity();     //Devuelve la humedad en porcentaje
      float t = dht.readTemperature();  //devuelve la temperatura en grados Celsius
      // Si no se puede leer la humedad o la temperatura, las funciones devuelven NAN
      if (isnan(h) || isnan(t)) {  //isnan() comprueba si dan NAN | \t es un carácter de tabulación, que nos permite alinear los datos en el monitor serie |
        mySerial.println("Error al leer el sensor DHT11");
      } else {
        mySerial.print("T:");
        mySerial.print(t);
        mySerial.print(":H:");
        mySerial.print(h);
        digitalWrite(ledtr, HIGH);
        delay(100);
        digitalWrite(ledtr, LOW);
      }
    }
    nextMillis1 = millis() + interval1;
  }
}